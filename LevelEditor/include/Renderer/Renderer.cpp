#include "precomp.h"

#include "Renderer/Renderer.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "Shared/Game/SpriteTypes.hpp"

#include "Config/Config.h"

#include "Editor/Editor.h"

Renderer::Renderer(SDL_Renderer* _renderer) : m_Renderer(_renderer)
{
	for(const auto& sprite : Graphics::SpriteConfig::spriteMap()) 
	{
		LoadSprites(sprite);
	}
}

Renderer::~Renderer()
{
	for(const auto&[type, sprite] : m_Sprites)
	{
		SDL_DestroyTexture(sprite.Texture);
		SDL_FreeSurface(sprite.Surface);
	}

	m_Sprites.clear();
	m_RenderQuery.clear();
}

void Renderer::LoadSprites(const Graphics::SpriteArgs& _args)
{
	std::string path = "assets";
	path.append("/sprites/");
	path.append(_args.Path);

	const char* asset_path = path.c_str();

	DEVIOUS_ASSERT(m_Sprites.find(_args.Type) == m_Sprites.end());
	SDL_Surface* surface = IMG_Load(asset_path);

	if (surface == nullptr)
	{
		fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
		DEVIOUS_ASSERT(surface == nullptr);
	}

	//Store the surface details and lock it behind the sprite type.
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_Renderer, surface);

	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	m_Sprites[_args.Type] = Sprite(texture, surface, _args.FrameCount);
}

void Renderer::DrawGrid()
{
	using namespace App::Config;
	using namespace Utilities;

	const ivec2 windowSize = { Editor::s_WindowWidth, Editor::s_WindowHeight };

	const ivec2 camPos = g_globals.Camera->Position;
	const ivec2 offset =
	{
		-(camPos.x % GRIDSIZE),
		-(camPos.y % GRIDSIZE)
	};

	const U32 camZoom = g_globals.Camera->Zoom;
	const U32 modGridSize = GRIDSIZE * camZoom;

	//*---------------------
	// Draw infinite grid.
	//*
	{
		SDL_SetRenderDrawColor(m_Renderer, 100, 100, 100, 100);

		const ivec2 start = offset * camZoom;

		for (U32 x = start.x; x < windowSize.x; x += modGridSize)
		{
			SDL_RenderDrawLine(m_Renderer, x, 0, x, windowSize.y);
		}

		for (U32 y = start.y; y < windowSize.y; y += modGridSize)
		{

			SDL_RenderDrawLine(m_Renderer, 0, y, windowSize.x, y);
		}
	}
}

const bool Renderer::IsVisible(const SDL_Rect& _rect) const
{
	const Utilities::ivec2 windowSize = { Editor::s_WindowWidth, Editor::s_WindowHeight };

	const SDL_Rect screen
	{
		0,
		0,
		windowSize.x,
		windowSize.y
	};

	return SDL_HasIntersection(&_rect, &screen);
}

const Opt<Sprite> Renderer::GetSprite(const Graphics::SpriteType& _type)
{
	if(m_Sprites.find(_type) != m_Sprites.end()) 
	{
		return m_Sprites[_type];
	}
	
	return std::nullopt;
}

void Renderer::DrawRect(const SDL_Rect& _rect, const Color& _col, const U8& _zOrder)
{
	SDL_Texture* texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _rect.w, _rect.h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(m_Renderer, texture);

	SDL_RenderClear(m_Renderer);

	SDL_SetRenderDrawColor(m_Renderer, _col.R, _col.G, _col.B, _col.A);

	SDL_RenderFillRect(m_Renderer, nullptr);

	SDL_SetRenderTarget(m_Renderer, nullptr);

	RenderQueryInstance instance; 
	{
		instance.Color = _col;
		instance.Type = Graphics::SpriteType::NONE;
		instance.Position = Utilities::ivec2(_rect.x, _rect.y);
		instance.Size = Utilities::ivec2(_rect.w, _rect.h);
		instance.Texture = texture;
		instance.Frame = 0;
		instance.Flags = e_TextureFlags::TEXTURE_DESTROY_AFTER_USE;
	}

	m_RenderQuery[_zOrder].push_back(instance);
}

void Renderer::StartFrame()
{
	SDL_SetRenderDrawColor(m_Renderer, 50, 50, 50, 255);
	SDL_RenderClear(m_Renderer);

	DrawGrid();
}

void Renderer::EndFrame() 
{
	Ref<Camera> camera = g_globals.Camera;

	for (auto& [zOrder, renderList] : m_RenderQuery)
	{
		for (auto& queryItem : renderList)
		{
			const SDL_Rect dstRect
			{
				(queryItem.Position.x - camera->Position.x) * camera->Zoom,
				(queryItem.Position.y - camera->Position.y) * camera->Zoom,
				 queryItem.Size.x * camera->Zoom,
				 queryItem.Size.y * camera->Zoom
			};

			if (!IsVisible(dstRect))
			{
				continue;
			}

			U32 texWidth, texHeight;
			SDL_QueryTexture(queryItem.Texture, nullptr, nullptr, &texWidth, &texHeight);

			U32 frame = 0;
			U32 spriteWidth = 0;
			{
				if (queryItem.Type == Graphics::SpriteType::NONE)
				{
					spriteWidth = texWidth;
					frame = 0;
				}
				else
				{
					Sprite& sprite = m_Sprites[queryItem.Type];
					spriteWidth = sprite.FrameCount == 0 ? texWidth : texWidth / sprite.FrameCount;
					frame = std::clamp<U32>(queryItem.Frame, 0, sprite.FrameCount);
				}
			}

			const SDL_Rect srcRect
			{
				frame == 0 ? 0 : spriteWidth * frame,
				0,
				spriteWidth,
				texHeight,
			};

			SDL_SetTextureColorMod(queryItem.Texture, queryItem.Color.R, queryItem.Color.G, queryItem.Color.B);

			SDL_RenderCopy(m_Renderer, queryItem.Texture, &srcRect, &dstRect);

			SDL_SetTextureColorMod(queryItem.Texture, 255, 255, 255);

			if (queryItem.Flags & e_TextureFlags::TEXTURE_DESTROY_AFTER_USE)
			{
				SDL_DestroyTexture(queryItem.Texture);
			}
		}
	}

	m_RenderQuery.clear();
}

void Renderer::Render(const RenderQuery& _query, const U8& _zOrder)
{
	if (_query.Type == Graphics::SpriteType::NONE)
		return;

	RenderQueryInstance instance;
	instance.Color    = _query.Color;
	instance.Type     = _query.Type;
	instance.Frame    = _query.Frame;
	instance.Position = _query.Position;
	instance.Size     = _query.Size;
	instance.Flags    = e_TextureFlags::TEXTURE_NONE;
	instance.Texture  = m_Sprites[_query.Type].Texture;

	m_RenderQuery[_zOrder].push_back(instance);
}
