#include "precomp.h"

#include "Shared/Utilities/UUID.hpp"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Network/Client/ClientWrapper.h"

#include <random>

#include <filesystem>

using namespace DEVIOUSMUD;
using namespace RANDOM;

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
	camera = _camera;
}

std::shared_ptr<Camera>& Graphics::Renderer::get_camera()
{
	return camera;
}

Graphics::Renderer::Renderer(SDL_Window* _window, SDL_Renderer* _renderer, const std::string& _texture_path) :
	window(_window), renderer(_renderer), texturepath(_texture_path)
{
	//Make the window resizable
	SDL_SetWindowResizable(_window, SDL_bool(true));

	std::cout << "[SDL_Renderer] Succesfully created renderer." << std::endl;

	camera = std::make_shared<Camera>();
}

Graphics::Renderer::~Renderer()
{
	free();
}

void Graphics::Renderer::free()
{
	//Clean up all textures & surfaces
	for (std::pair<SpriteType, SDL_SpriteDetails*> details : sprites)
	{
		delete details.second;
	}   sprites.clear();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}

/// <summary>
/// Binds a sprite type to a surface.
/// </summary>
/// <param name="_file"></param>
/// <param name="_spritetype"></param>
/// <param name="_maxframecount"></param>
void Graphics::Renderer::load_and_bind_surface(const std::string& _file, const Graphics::SpriteType& _spritetype, const uint32_t& _maxframecount)
{
	std::string path = texturepath;
	path.append(_file);

	const char* asset_path = path.c_str();

	assert(sprites.find(_spritetype) == sprites.end());
	SDL_Surface* surface = IMG_Load(asset_path);

	//Check if the image got loaded correctly.
	if (surface == nullptr)
	{
		fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
		assert(surface == nullptr);
	}

	//Store the surface details and lock it behind the sprite type.
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	sprites[_spritetype] = new SDL_SpriteDetails(surface, texture, _maxframecount);
}

void Graphics::Renderer::start_frame()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}

void Graphics::Renderer::draw_outline(const Utilities::vec2& _pos, const Utilities::vec2& _size, int _borderWidth, SDL_Color _color)
{
	SDL_Rect topRect = { _pos.x - _borderWidth, _pos.y - _borderWidth, _size.x + _borderWidth * 2, _borderWidth };
	SDL_Rect bottomRect = { _pos.x - _borderWidth, _pos.y + _size.y, _size.x + _borderWidth * 2, _borderWidth };
	SDL_Rect leftRect = { _pos.x - _borderWidth, _pos.y, _borderWidth, _size.y };
	SDL_Rect rightRect = { _pos.x + _size.x, _pos.y, _borderWidth, _size.y };
	
	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
	
	SDL_RenderFillRect(renderer, &topRect);
	SDL_RenderFillRect(renderer, &bottomRect);
	SDL_RenderFillRect(renderer, &leftRect);
	SDL_RenderFillRect(renderer, &rightRect);
}

/// <summary>
/// This function plots the given sprite ontop of the canvas, call endframe when you have build the final frame that you want to render.
/// </summary>
/// <param name="(s)">, Sprite that you want to render ontop of the canvas.</param>
/// <param name="(pos)"></param>
/// <param name="(scale)"></param>
void Graphics::Renderer::plot_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size, const int32_t _gridsize)
{
	if (_s.get_sprite_type() == SpriteType::NONE)
		return;

	if (camera)
	{
		// Get the viewport size.
		int32_t windowSize_w, windowSize_h;
		{
			get_viewport_size(&windowSize_w, &windowSize_h);
		}

		const float vpWidthHalfExtends = (float)windowSize_w / 2.0f;
		const float vpHeightHalfExtends = (float)windowSize_h / 2.0f;

		// Create a new rect with the given size and transformed position.
		const Utilities::vec2 camPos = camera->get_position();
		const SDL_Rect destRect
		{
			(int32_t)roundf((_pos.x * (float)_gridsize) - camPos.x + vpWidthHalfExtends),
			(int32_t)roundf((_pos.y * (float)_gridsize) - camPos.y + vpHeightHalfExtends),
			(int32_t)roundf(_size.x),
			(int32_t)roundf(_size.y)
		};

		//Get texture
		SDL_Texture* texture = sprites[_s.get_sprite_type()]->get_texture();

		// Get texture width and height.
		int32_t imageWidth, imageHeight;
		{
			SDL_QueryTexture(texture, NULL, NULL, &imageWidth, &imageHeight);
		}

		// Generate rect based on what part of the png we want our sprite to be 
		const int32_t spriteWidth = imageWidth / _s.get_framecount();
		const int32_t frame = CLAMP(_s.frame, 0, _s.get_framecount());

		const SDL_Rect srcRect
		{
			spriteWidth * frame,
			0,
			spriteWidth,
			imageHeight,
		};

		//Set colors.
		SDL_SetTextureColorMod(texture, _s.color.r, _s.color.b, _s.color.g);
		SDL_SetTextureAlphaMod(texture, _s.color.a);

		//Copy it over to the final frame.
		SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

		//Reset to default colors after rendering.
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
}

void Graphics::Renderer::plot_raw_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size)
{
	if (_s.get_sprite_type() == SpriteType::NONE)
		return;

	//Get texture
	SDL_Texture* texture = sprites[_s.get_sprite_type()]->get_texture();

	// Generate rect based on what part of the png we want our sprite to be 
	const int32_t spriteWidth = (int32_t)_s.size.x / _s.get_framecount();
	const int32_t frame = CLAMP(_s.frame, 0, _s.get_framecount());

	const SDL_Rect srcRect
	{
		spriteWidth * frame,
		0,
		spriteWidth,
		_s.size.y,
	};

	const SDL_Rect destRect
	{
		(int32_t)roundf(_pos.x),
		(int32_t)roundf(_pos.y),
		(int32_t)roundf(_size.x),
		(int32_t)roundf(_size.y)
	};

	//Set colors.
	SDL_SetTextureColorMod(texture, _s.color.r, _s.color.b, _s.color.g);
	SDL_SetTextureAlphaMod(texture, _s.color.a);

	//Copy it over to the final frame.
	SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

	//Reset to default colors after rendering.
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Graphics::Renderer::get_viewport_size(int32_t* _w, int32_t* _h)
{
	SDL_GetWindowSize(window, _w, _h);
}

/// <summary>
/// Renders the frame as final.
/// </summary>
void Graphics::Renderer::end_frame()
{
	SDL_RenderPresent(renderer);
}

Graphics::Sprite Graphics::Renderer::get_sprite(const SpriteType& _spriteType)
{
	if(_spriteType == SpriteType::NONE) 
	{
		Sprite sprite;
		sprite.color = SDL_Color{ 255, 255, 255, 255 };
		sprite.frame = 0;
		sprite.frameCount = 0;
		sprite.spriteType = _spriteType;
		sprite.size = Utilities::vec2(0.0f, 0.0f);
		return sprite;
	}

	auto s_iterator = sprites.find(_spriteType);
	DEVIOUS_ASSERT(s_iterator != sprites.end())

	//Default sprite color.
	const SDL_Color color = { 255, 255, 255, 255 };

	//Grab the surface details
	const SDL_SpriteDetails* details = sprites[_spriteType];

	Sprite sprite;
	sprite.color      = SDL_Color{ 255, 255, 255, 255 };
	sprite.frame      = 0;
	sprite.frameCount = details->get_framecount();
	sprite.spriteType = _spriteType;
	sprite.size       = Utilities::vec2((float)details->get_surface()->w, (float)details->get_surface()->h);

	return sprite;
}

Graphics::Renderer::SDL_SpriteDetails::SDL_SpriteDetails(SDL_Surface* _surface, SDL_Texture* _texture, const uint32_t& _framecount) 
	: surface(_surface), framecount(_framecount), texture(_texture)
{

}

Graphics::Renderer::SDL_SpriteDetails::~SDL_SpriteDetails()
{
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

const uint32_t& Graphics::Renderer::SDL_SpriteDetails::get_framecount() const
{
	return framecount;
}

SDL_Surface* Graphics::Renderer::SDL_SpriteDetails::get_surface() const
{
	return surface;
}

SDL_Texture* Graphics::Renderer::SDL_SpriteDetails::get_texture()
{
	return texture;
}


