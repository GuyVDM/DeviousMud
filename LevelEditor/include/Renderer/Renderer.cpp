#include "precomp.h"

#include "Renderer/Renderer.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

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

void Renderer::DrawRect(const SDL_Rect& _rect, const Color& _col)
{
	SDL_SetRenderDrawColor(m_Renderer, _col.R, _col.G, _col.B, _col.A);
	SDL_RenderFillRect(m_Renderer, &_rect);
}

void Renderer::DrawRectViewProjection(const SDL_Rect& _rect, const Color& _col)
{
	Ref<Camera> camera = g_globals.Camera;

	const SDL_Rect dstRect
	{
		(_rect.x - camera->Position.x) * camera->Zoom,
		(_rect.y - camera->Position.y) * camera->Zoom,
		(_rect.w) * camera->Zoom,
		(_rect.h) * camera->Zoom
	};

	SDL_SetRenderDrawColor(m_Renderer, _col.R, _col.G, _col.B, _col.A);
	SDL_RenderFillRect(m_Renderer, &dstRect);
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

	for(auto&[zOrder, queryItem] : m_RenderQuery)
	{
		const SDL_Rect dstRect
		{
			(queryItem.Position.x - camera->Position.x) * camera->Zoom,
			(queryItem.Position.y - camera->Position.y) * camera->Zoom,
			(queryItem.Size.x) * camera->Zoom,
			(queryItem.Size.y) * camera->Zoom
		};

		if(!IsVisible(dstRect)) 
		{
			continue;
		}

		Sprite& sprite = m_Sprites[queryItem.Type];

		U32 texWidth, texHeight;
		SDL_QueryTexture(sprite.Texture, nullptr, nullptr, &texWidth, &texHeight);

		const U32 spriteWidth = sprite.FrameCount == 0 ? texWidth : texWidth / sprite.FrameCount;
		const U32 frame = std::clamp<U32>(queryItem.Frame, 0, sprite.FrameCount);

		const SDL_Rect srcRect
		{
			frame == 0 ? 0 : spriteWidth * frame,
			0,
			spriteWidth,
			texHeight,
		};

		SDL_SetTextureColorMod(sprite.Texture, queryItem.Color.R, queryItem.Color.G, queryItem.Color.B);

		SDL_RenderCopy(m_Renderer, sprite.Texture, &srcRect, &dstRect);

		SDL_SetTextureColorMod(sprite.Texture, 255, 255, 255);
	}

	m_RenderQuery.clear();
}

void Renderer::Render(const RenderQuery& _query, const U8& _zOrder)
{
	if (_query.Type == Graphics::SpriteType::NONE)
		return;

	m_RenderQuery[_zOrder] = _query;
}
