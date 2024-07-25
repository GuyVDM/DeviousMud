#include "precomp.h"

#include "Core/Renderer/Renderer.h"

#include "Core/Camera/Camera.h"
#include "Core/Renderer/FontLoader/FontLoader.h"
#include "Core/Globals/Globals.h"
#include "Core/Config/Config.h"
#include "Core/Editor/Editor.h"
#include "Core/Renderer/SubSprite/SubSprite.hpp"

Renderer::Renderer(const Utilities::ivec2& _windowSize)
{
	m_Window   = nullptr;
	m_Renderer = nullptr;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
	TTF_Init();

	CreateSDLWindow(_windowSize.x, _windowSize.y);

	SDL_Surface* editorIconSurface = IMG_Load("icons/windowicon.png");
	SDL_SetWindowIcon(m_Window, editorIconSurface);
	SDL_FreeSurface(editorIconSurface);

	for(const auto& sprite : Graphics::SpriteConfig::spriteMap()) 
	{
		LoadSprites(sprite);
	}	CreateRectTexture();

	m_FontLoader = std::make_shared<FontLoader>(App::Config::s_FontPath);

}

Renderer::~Renderer()
{
	//*-----------------
	// Clean up textures.
	//
	{
		for (const auto& [type, sprite] : m_Sprites)
		{
			SDL_DestroyTexture(sprite.Texture);
			SDL_FreeSurface(sprite.Surface);
		}   m_Sprites.clear();

		m_RenderQuery.clear();
		SDL_DestroyTexture(m_RectTexture);
	}
	
	//*-----------------
	// Destroy ttf font.
	//
	{
		m_FontLoader = nullptr;
	}

	//*--------------------
	// Quit SDL Subsystems.
	//
	SDL_DestroyWindow(m_Window);
	SDL_DestroyRenderer(m_Renderer);
	TTF_Quit();
}

void Renderer::LoadSprites(const Graphics::SpriteArgs& _args)
{
	std::string path = "assets/sprites/";
	path.append(_args.Path);

	DEVIOUS_ASSERT(m_Sprites.find(_args.Type) == m_Sprites.end());
	SDL_Surface* surface = IMG_Load(path.c_str());

	if (surface == nullptr)
	{
		DEVIOUS_ERR("Error loading image: " << IMG_GetError());
		DEVIOUS_ASSERT(surface == nullptr);
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
	if (texture == nullptr)
	{
		DEVIOUS_ERR("Error loading texture: " << IMG_GetError());
		DEVIOUS_ASSERT(texture == nullptr);
	} 

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	const U32 frameCount = _args.Rows * _args.Columns;
	m_Sprites[_args.Type] = Sprite(texture, surface, frameCount, _args.Rows, _args.Columns);
}

void Renderer::DrawGrid()
{
	using namespace App::Config;
	using namespace Utilities;

	const ivec2 windowSize = { Editor::s_WindowWidth, Editor::s_WindowHeight };

	const ivec2 camPos = g_globals.Camera->Position;
	const ivec2 offset =
	{
		-(camPos.x % GRIDCELLSIZE),
		-(camPos.y % GRIDCELLSIZE)
	};

	const I32 camZoom = g_globals.Camera->Zoom;
	const I32 modGridSize = GRIDCELLSIZE * camZoom;

	//*---------------------
	// Draw infinite grid.
	//*
	{
		SDL_SetRenderDrawColor(m_Renderer, 100, 100, 100, 100);

		const ivec2 start = offset * camZoom;

		for (I32 x = start.x; x < windowSize.x; x += modGridSize)
		{
			SDL_RenderDrawLine(m_Renderer, x, 0, x, windowSize.y);
		}

		for (I32 y = start.y; y < windowSize.y; y += modGridSize)
		{

			SDL_RenderDrawLine(m_Renderer, 0, y, windowSize.x, y);
		}
	}
}

void Renderer::CreateRectTexture()
{
	constexpr U32 rmask = 0xff000000,
		          gmask = 0x00ff0000, 
		          bmask = 0x0000ff00,
		          amask = 0x000000ff;

	SDL_Surface*             surface = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	const SDL_PixelFormat*   fmt = surface->format;

	constexpr SDL_Rect rect  = { 0, 0, 1, 1 };
	constexpr Color    col   = { 255, 255, 255, 255 };
	const     U32      pixel = SDL_MapRGBA(fmt, col.R, col.G, col.B, col.A);

	SDL_FillRect(surface, &rect, pixel);

	m_RectTexture = SDL_CreateTextureFromSurface(m_Renderer, surface);

	SDL_SetTextureBlendMode(m_RectTexture, SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(surface);
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

const Utilities::ivec2 Renderer::ScreenToWorld(const Utilities::ivec2& _screenCoords)
{
	Ref<Camera> camera = g_globals.Camera;

	if (camera->Zoom == 0)
	{
		return Utilities::ivec2(0, 0);
	}

	return Utilities::ivec2
	(
		static_cast<U32>((_screenCoords.x / camera->Zoom) + camera->Position.x),
		static_cast<U32>((_screenCoords.y / camera->Zoom) + camera->Position.y)
	);
}

Optional<Sprite> Renderer::GetSprite(const Graphics::SpriteType& _type)
{
	if(m_Sprites.find(_type) != m_Sprites.end()) 
	{
		return m_Sprites[_type];
	}
	
	return std::nullopt;
}

Optional<SpriteFrameArgs> Renderer::GetSpriteFrameInfo(const SubSprite& _subsprite)
{
	Optional<Sprite> optSprite = GetSprite(_subsprite.SpriteType);

	if (!optSprite.has_value())
	{
		return std::nullopt;
	}

	Sprite& sprite = optSprite.value();

	I32 texWidth, texHeight;
	SDL_QueryTexture(sprite.Texture, nullptr, nullptr, &texWidth, &texHeight);

	Utilities::ivec2 spriteUV(0), spriteSize(texWidth, texHeight);
	{
		Sprite& sprite = m_Sprites[_subsprite.SpriteType];
		bool bIsSpriteSheet = (sprite.Rows != 0 && sprite.Columns != 0);

		if (bIsSpriteSheet)
		{
			const U32 targetColumn = _subsprite.Frame % sprite.Columns;
			const U32 targetRow    = _subsprite.Frame / sprite.Columns;

			spriteSize.x = texWidth     / sprite.Columns;
			spriteSize.y = texHeight    / sprite.Rows;
			spriteUV.x   = targetColumn * spriteSize.x; 
			spriteUV.y   = targetRow    * spriteSize.y; 
		}
	}

	return SpriteFrameArgs(spriteUV, spriteSize, sprite.FrameCount);
}

void Renderer::DrawRect(const SDL_Rect& _rect, const Color& _col, const U8& _zOrder)
{
	RenderQueryInstance instance =
	{
		/* Type     */ Graphics::SpriteType::NONE,
		/* Position */ Utilities::ivec2(_rect.x, _rect.y),
		/* Size     */ Utilities::ivec2(_rect.w, _rect.h),
		/* Color    */ _col,
		/* Frame    */ 0,
		/* Texture  */ m_RectTexture,
		/* Flags    */ e_TextureFlags::TEXTURE_NONE,
	};
	
	m_RenderQuery[_zOrder].push_back(instance);
}

void Renderer::DrawRectOutline(const SDL_Rect& _rect, const Color& _col, const I32& _outlWidth, const U8& _zOrder)
{
	const SDL_Rect top =
	{
		_rect.x,
		_rect.y,
		_rect.w,
		_outlWidth
	};

	DrawRect(top, _col, _zOrder);

	const SDL_Rect bot =
	{
		_rect.x,
		_rect.y + _rect.h - _outlWidth,
		_rect.w,
		_outlWidth
	};

	DrawRect(bot, _col, _zOrder);

	const SDL_Rect left =
	{
		_rect.x,
		_rect.y,
		_outlWidth,
		_rect.h
	};

	DrawRect(left, _col, _zOrder);

	const SDL_Rect right =
	{
		_rect.x + _rect.w - _outlWidth,
		_rect.y,
		_outlWidth,
		_rect.h
	};

	DrawRect(right, _col, _zOrder);
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

	std::vector<SDL_Texture*> toDestroy;

	for (const auto& [zOrder, renderList] : m_RenderQuery)
	{
		for (const auto& queryItem : renderList)
		{
			if (queryItem.Flags & e_TextureFlags::TEXTURE_DESTROY_AFTER_USE)
			{
				toDestroy.push_back(queryItem.Texture);
			}

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

			//Calculate spritesheet UV's and individual sprite size.
			Utilities::ivec2 spriteUV(0), spriteSize(0, 0);
			{
				Optional<SpriteFrameArgs> optSpriteInfo = GetSpriteFrameInfo(SubSprite(queryItem.SpriteType, queryItem.Frame));

				if(optSpriteInfo.has_value()) 
				{
					SpriteFrameArgs& spriteInfo = optSpriteInfo.value();
					spriteUV   = spriteInfo.UV;
					spriteSize = spriteInfo.IndividualSpriteSize;
				}
				else 
				{
					SDL_QueryTexture(queryItem.Texture, nullptr, nullptr, &spriteSize.x, &spriteSize.y);
				}
			}

			const SDL_Rect srcRect
			{
				spriteUV.x,
				spriteUV.y,
				spriteSize.x,
				spriteSize.y
			};

			SDL_SetTextureColorMod(queryItem.Texture, queryItem.Color.R, queryItem.Color.G, queryItem.Color.B);
			SDL_SetTextureAlphaMod(queryItem.Texture, queryItem.Color.A);

			SDL_RenderCopy(m_Renderer, queryItem.Texture, &srcRect, &dstRect);

			SDL_SetTextureColorMod(queryItem.Texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(queryItem.Texture, 255);
		}
	}

	for(SDL_Texture* tex : toDestroy) 
	{
		SDL_DestroyTexture(tex);
	}

	m_RenderQuery.clear();

	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_Renderer);

	SDL_RenderPresent(m_Renderer);
}

void Renderer::RenderText(const TextArgs& _args, const float& _scale, const e_TextAlignment& _alignment)
{
	SDL_Texture* textTexture = m_FontLoader->TextToTexture(m_Renderer, _args.Text.c_str(), _args.TextSize);
	if(!textTexture) 
	{
		DEVIOUS_ERR("Could not load texture.");
		return;
	}

	Utilities::ivec2 textureDimensions = {};
	SDL_QueryTexture(textTexture, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

	RenderQueryInstance query;
	query.Color      = _args.Color;
	query.Flags      = e_TextureFlags::TEXTURE_DESTROY_AFTER_USE;
	query.Frame      = 0;
	query.Position   = _args.Position;
	query.Size       = textureDimensions * _scale;
	query.Texture    = textTexture;
	query.SpriteType = Graphics::SpriteType::NONE;

	switch(_alignment) 
	{
		case e_TextAlignment::MIDDLE:
		{
			query.Position.x = query.Position.x - static_cast<U32>(roundf(static_cast<float>(query.Size.x) * 0.5f));
		}
		break;

		case e_TextAlignment::LEFT:
		{
			query.Position.x = query.Position.x - query.Size.x;
		}
		break;
	}

	m_RenderQuery[_args.ZOrder].push_back(query);

}

void Renderer::CreateSDLWindow(const I32& _width, const I32& _height)
{
	U32 flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	m_WindowName = "DM Editor";

	if (_width == 0 && _height == 0)
	{
		flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
		m_Window = SDL_CreateWindow("DM Editor - Untitled Map", 0, 0, 1000, 1000, flags);
	}
	else
	{
		m_Window = SDL_CreateWindow("DM Editor - Untitled Map", 0, 0, _width, _height, flags);
	}

	if (!m_Window)
	{
		DEVIOUS_LOG("Window couldn't get created: " << SDL_GetError());
	}

	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

SDL_Window* Renderer::GetWindow()
{
	return m_Window;
}

SDL_Renderer* Renderer::GetRenderer()
{
	return m_Renderer;
}

void Renderer::UpdateMapName(const std::string& _mapFilePath)
{
	std::string fileName = "Untitled Map";

	if(!_mapFilePath.empty()) 
	{
		fileName = _mapFilePath;
	}

	std::string windowName = m_WindowName;
	windowName.append(" - ");
	windowName.append(fileName);

	SDL_SetWindowTitle(m_Window, windowName.c_str());
}

void Renderer::Render(const RenderQuery& _query, const U8& _zOrder)
{
	if (_query.SpriteType == Graphics::SpriteType::NONE)
	{
		return;
	}

	RenderQueryInstance instance;
	instance.Color      = _query.Color;
	instance.SpriteType = _query.SpriteType;
	instance.Frame      = _query.Frame;
	instance.Position   = _query.Position;
	instance.Size       = _query.Size;
	instance.Flags      = e_TextureFlags::TEXTURE_NONE;
	instance.Texture    = m_Sprites[_query.SpriteType].Texture;

	m_RenderQuery[_zOrder].push_back(instance);
}

