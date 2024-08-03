#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Network/Client/ClientWrapper.h"

#include "Shared/Utilities/UUID.hpp"

#include <random>

#include <filesystem>

using namespace DM;
using namespace Utils;

Graphics::Renderer* Graphics::Renderer::create_renderer(const char* _title, const Utilities::ivec2& _c_scale, const std::string& _texture_path)
{
	SDL_Window* w  = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _c_scale.x, _c_scale.y, 0);
	SDL_Renderer* r = SDL_CreateRenderer (w, -1, SDL_RENDERER_ACCELERATED);

	//Enable alpha blending
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

	return new Graphics::Renderer(w, r, _texture_path);
}

void Graphics::Renderer::set_camera(std::shared_ptr<Camera>& _camera)
{
	DEVIOUS_ASSERT(_camera != nullptr);
	m_camera = _camera;
}

std::shared_ptr<Camera>& Graphics::Renderer::get_camera()
{
	return m_camera;
}

Graphics::Renderer::Renderer(SDL_Window* _window, SDL_Renderer* _renderer, const std::string& _texture_path) :
	m_window(_window), m_renderer(_renderer), m_assetsPath(_texture_path)
{
	//Make the window resizable
	SDL_SetWindowResizable(_window, SDL_bool(true));
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	std::cout << "[SDL_Renderer] Succesfully created renderer." << std::endl;

	m_camera = std::make_shared<Camera>();
}

Graphics::Renderer::~Renderer()
{
	free();
}

void Graphics::Renderer::free()
{
	//Clean up all textures & surfaces
	for (std::pair<SpriteType, SDL_SpriteDetails*> m_details : m_sprites)
	{
		delete m_details.second;
	}   m_sprites.clear();

	m_renderQueue.clear();

	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
}

/// <summary>
/// Binds a sprite type to a surface.
/// </summary>
/// <param name="_file"></param>
/// <param name="_spritetype"></param>
/// <param name="_maxframecount"></param>
void Graphics::Renderer::load_and_bind_surface(const std::string& _file, const Graphics::SpriteType& _spritetype, const uint32_t& _rows, const uint32_t& _columns)
{
	std::string m_path = m_assetsPath;
	m_path.append("/sprites/");
	m_path.append(_file);

	const char* asset_path = m_path.c_str();

	DEVIOUS_ASSERT(m_sprites.find(_spritetype) == m_sprites.end());
	SDL_Surface* surface = IMG_Load(asset_path);

	//Check if the image got loaded correctly.
	if (surface == nullptr)
	{
		fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
		DEVIOUS_ASSERT(surface == nullptr);
	}

	//Store the surface details and lock it behind the sprite type.
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	m_sprites[_spritetype] = new SDL_SpriteDetails(surface, texture, _rows, _columns);
}

void Graphics::Renderer::start_frame()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);
}

void Graphics::Renderer::draw_rect(const Utilities::vec2 _pos, const Utilities::vec2 _size, SDL_Color _color, uint8_t _zOrder, bool _bScreenspace)
{
	SDL_Texture* texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
		static_cast<int>(_size.x),
		static_cast<int>(_size.y)
	);

	//Create rect texture.
	{
		const Utilities::ivec2 pos  = Utilities::to_ivec2(_pos);
		const Utilities::ivec2 size = Utilities::to_ivec2(_size);

		const SDL_Rect rect = 
		{
			0,
			0,
			size.x,
			size.y
		};

		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(m_renderer, texture);
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);

		SDL_RenderClear(m_renderer);

		SDL_SetRenderDrawColor(m_renderer, _color.r, _color.g, _color.b, _color.a);

		SDL_RenderFillRect(m_renderer, &rect);

		SDL_SetRenderTarget(m_renderer, NULL);
	}

	//Render flags
	uint8_t flags = SpriteRenderData::e_SpriteRenderFlags::RENDER_DESTROY_TEXTURE_AFTER_USE;
	if (!_bScreenspace)
	{
		flags |= SpriteRenderData::e_SpriteRenderFlags::RENDER_WORLDSPACE;
	}

	SpriteRenderData data;
	data.position    = _pos;
	data.size        = _size;
	data.texture     = texture;
	data.frame       = 0;
	data.frameCount  = 0;
	data.rows        = 0;
	data.columns     = 0;
	data.renderFlags = flags;

	m_renderQueue[_zOrder].push_back(std::move(data));
}

void Graphics::Renderer::draw_outline(const Utilities::vec2& _pos, const Utilities::vec2& _size, int _borderWidth, SDL_Color _color, uint8_t _zOrder, bool _bScreenspace)
{
	const Utilities::ivec2 size = Utilities::to_ivec2(_size);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);

	//Create a texture with the desired size and format
	SDL_Texture* texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x + (2 * _borderWidth), size.y + (2 * _borderWidth));
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(m_renderer, texture);

	//Set rect color
	SDL_SetRenderDrawColor(m_renderer, _color.r, _color.g, _color.b, _color.a);

	const SDL_Rect top =
	{
		0,
		0,
		size.x + 2 * _borderWidth,
		_borderWidth
	};
	SDL_RenderFillRect(m_renderer, &top);

	const SDL_Rect bottom =
	{
		0,
		size.y + _borderWidth,
		size.x + 2 * _borderWidth,
		_borderWidth
	};
	SDL_RenderFillRect(m_renderer, &bottom);

	const SDL_Rect left =
	{
		0,
		_borderWidth,
		_borderWidth,
		size.y
	};
	SDL_RenderFillRect(m_renderer, &left);

	const SDL_Rect right =
	{
		size.x + _borderWidth,
		_borderWidth,
		_borderWidth,
		size.y
	};
	SDL_RenderFillRect(m_renderer, &right);

	SDL_SetRenderTarget(m_renderer, NULL);

	//Set flags
	uint8_t flags = SpriteRenderData::e_SpriteRenderFlags::RENDER_DESTROY_TEXTURE_AFTER_USE;
	if (!_bScreenspace)
	{
		flags |= SpriteRenderData::e_SpriteRenderFlags::RENDER_WORLDSPACE;
	}

	// Reset the rendering target to the default
	SpriteRenderData data;
	data.position    = _pos;
	data.size        = _size;
	data.texture	 = texture;
	data.frameCount  = 0;
	data.frame		 = 0;
	data.rows		 = 0;
	data.columns     = 0;
	data.renderFlags = flags;

	m_renderQueue[_zOrder].push_back(std::move(data));
}

void Graphics::Renderer::plot_texture(SpriteRenderData& _data, uint8_t _zOrder)
{
	m_renderQueue[_zOrder].push_back(std::move(_data));
}

/// <summary>
/// NOTE: Plotting the frame is relative to the camera, were using worldspace coordinates.
/// This function plots the given sprite ontop of the canvas, call endframe when you have build the final frame that you want to render.
/// </summary>
/// <param name="(s)">, Sprite that you want to render ontop of the canvas.</param>
/// <param name="(pos)"></param>
/// <param name="(scale)"></param>
void Graphics::Renderer::plot_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size)
{
	if (_s.get_sprite_type() == SpriteType::NONE) 
	{
		return;
	}

	uint8_t flags = SpriteRenderData::e_SpriteRenderFlags::RENDER_WORLDSPACE;
	if (_s.bIsFlipped)
	{
		flags |= SpriteRenderData::e_SpriteRenderFlags::RENDER_FLIP_SPRITE;
	}

	SpriteRenderData renderData =
	{
		_pos,
		_size,
		m_sprites[_s.get_sprite_type()]->get_texture(),
		_s.frame,
		_s.get_framecount(),
		_s.m_rows,
		_s.m_columns,
		_s.color,
		flags
	};

	m_renderQueue[_s.zRenderPriority].push_back(std::move(renderData));
}

void Graphics::Renderer::plot_raw_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size)
{
	if (_s.get_sprite_type() == SpriteType::NONE)
	{
		return;
	}
	
	uint8_t flags = 0;

	if (_s.bIsFlipped)
	{
		flags = SpriteRenderData::e_SpriteRenderFlags::RENDER_FLIP_SPRITE;
	}

	SpriteRenderData renderData =
	{
		_pos,
		_size,
		m_sprites[_s.get_sprite_type()]->get_texture(),
		_s.frame,
		_s.get_framecount(),
		_s.m_rows,
		_s.m_columns,
		_s.color,
		flags
	};

	m_renderQueue[_s.zRenderPriority].push_back(std::move(renderData));
}

void Graphics::Renderer::get_viewport_size(int32_t* _w, int32_t* _h)
{
	SDL_GetWindowSize(m_window, _w, _h);
}

/// <summary>
/// Renders the frame as final.
/// </summary>
void Graphics::Renderer::end_frame()
{
	if (!m_camera)
	{
		return;
	}

	for (auto& [zRenderOrder, queue] : m_renderQueue)
	{
		for (const SpriteRenderData& data : queue)
		{
			SDL_Rect destRect{};

			///	Check if we need to render in screen or worldspace.
			/// Worldspace  == Relative to the camera, and exists within gridspace, defined by GRID_CELL_PX_SIZE.
			/// Screenspace == just uses direct screen coordinates.
			if (data.renderFlags & SpriteRenderData::e_SpriteRenderFlags::RENDER_WORLDSPACE)
			{
				// Get the viewport size.
				int32_t windowSize_w, windowSize_h;
				{
					get_viewport_size(&windowSize_w, &windowSize_h);
				}

				const float vpWidthHalfExtends  = static_cast<float>(windowSize_w / 2.0f);
				const float vpHeightHalfExtends = static_cast<float>(windowSize_h / 2.0f);
				const Utilities::vec2 camPos    = m_camera->get_position();

				// Create a new rect with the given size and transformed position.
				destRect =
				{
					static_cast<int32_t>(ceil((data.position.x * (float)GRID_CELL_PX_SIZE) - camPos.x + vpWidthHalfExtends)),
					static_cast<int32_t>(ceil((data.position.y * (float)GRID_CELL_PX_SIZE) - camPos.y + vpHeightHalfExtends)),
					static_cast<int32_t>(ceil(data.size.x)),
					static_cast<int32_t>(ceil(data.size.y))
				};
			}
			else
			{
				const Utilities::ivec2 spritePos  = Utilities::to_ivec2(data.position);
				const Utilities::ivec2 spriteSize = Utilities::to_ivec2(data.size);
				destRect =
				{
					spritePos.x,
					spritePos.y, 
					spriteSize.x,
					spriteSize.y 
				};
			}

			// Get texture width and height.
			int32_t imageWidth, imageHeight;
			SDL_QueryTexture(data.texture, NULL, NULL, &imageWidth, &imageHeight);

			// Generate rect based on what part of the png we want our sprite to be 
			Utilities::ivec2 spriteUV(0), spriteSize(imageWidth, imageHeight);
			{
				bool bIsSpriteSheet = (data.rows != 0 && data.columns != 0);

				if (bIsSpriteSheet)
				{
					const uint32_t targetColumn = data.frame % data.columns;
					const uint32_t targetRow    = data.frame / data.columns;

					spriteSize.x = imageWidth   / data.columns;
					spriteSize.y = imageHeight  / data.rows;
					spriteUV.x   = targetColumn * spriteSize.x;
					spriteUV.y   = targetRow    * spriteSize.y;
				}
			}

			const SDL_Rect srcRect
			{
				spriteUV.x,
				spriteUV.y,
				spriteSize.x,
				spriteSize.y,
			};

			///----------------------
			/// Building of the frame
			///
			{
				SDL_SetTextureColorMod(data.texture, data.color.r, data.color.g, data.color.b);
				SDL_SetTextureAlphaMod(data.texture, data.color.a);

				const SDL_RendererFlip flipFlags = (data.renderFlags & SpriteRenderData::e_SpriteRenderFlags::RENDER_FLIP_SPRITE) ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL :
																														            SDL_RendererFlip::SDL_FLIP_NONE;
				SDL_RenderCopyEx(m_renderer, data.texture, &srcRect, &destRect, 0, NULL, flipFlags);

				//Reset to default colors after copying it over to the surface.
				SDL_SetTextureColorMod(data.texture, 255, 255, 255);
			}

			if (data.renderFlags & SpriteRenderData::e_SpriteRenderFlags::RENDER_DESTROY_TEXTURE_AFTER_USE)
			{
				SDL_DestroyTexture(data.texture);
			}
		}

		queue.clear();
	}

	//Present the final frame.
	SDL_RenderPresent(m_renderer);
}

Graphics::Sprite Graphics::Renderer::get_sprite(const SpriteType& _spriteType)
{
	if(_spriteType == SpriteType::NONE) 
	{
		Sprite sprite;
		sprite.color = SDL_Color{ 255, 255, 255, 255 };
		sprite.frame = 0;
		sprite.m_frameCount = 0;
		sprite.m_eSpriteType = _spriteType;
		sprite.m_dimension = Utilities::vec2(0.0f, 0.0f);
		sprite.m_columns = 0;
		sprite.m_rows = 0;
		return sprite;
	}

	const auto it = m_sprites.find(_spriteType);
	DEVIOUS_ASSERT(it != m_sprites.end())

	//Default sprite color.
	const SDL_Color color = { 255, 255, 255, 255 };

	//Grab the surface details
	const SDL_SpriteDetails* details = m_sprites[_spriteType];

	Sprite sprite;
	sprite.color         = SDL_Color{ 255, 255, 255, 255 };
	sprite.frame         = 0;
	sprite.m_frameCount  = details->get_framecount();
	sprite.m_eSpriteType = _spriteType;
	sprite.m_dimension   = Utilities::vec2((float)details->get_surface()->w, (float)details->get_surface()->h);
	sprite.bIsFlipped    = false;
	sprite.m_rows        = details->get_rows();
	sprite.m_columns     = details->get_columns();

	return sprite;
}

Graphics::Renderer::SDL_SpriteDetails::SDL_SpriteDetails(SDL_Surface* _surface, SDL_Texture* _texture, const uint32_t& _rows, const uint32_t& _columns)
	: m_surface(_surface), m_texture(_texture), m_Rows(_rows), m_Columns(_columns)
{
	m_framecount = m_Rows * m_Columns;
}

Graphics::Renderer::SDL_SpriteDetails::~SDL_SpriteDetails()
{
	SDL_DestroyTexture(m_texture);
	SDL_FreeSurface(m_surface);
}

const uint32_t& Graphics::Renderer::SDL_SpriteDetails::get_framecount() const
{
	return m_framecount;
}

SDL_Surface* Graphics::Renderer::SDL_SpriteDetails::get_surface() const
{
	return m_surface;
}

SDL_Texture* Graphics::Renderer::SDL_SpriteDetails::get_texture()
{
	return m_texture;
}


