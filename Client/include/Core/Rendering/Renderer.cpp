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
	SDL_Window* w = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _c_scale.x, _c_scale.y, 0);
	SDL_Renderer* r = SDL_CreateRenderer (w, -1, 0);

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

	textures.resize(MAX_TEXTURE_COUNT);
	std::cout << "[SDL_Renderer] Succesfully created renderer." << std::endl;

	camera = std::make_shared<Camera>();
}

Graphics::Renderer::~Renderer()
{
	free();
}

void Graphics::Renderer::free()
{
	//Clean up all textures.
	for (SDL_Texture* t : textures)
	{
		SDL_DestroyTexture(t);
	}	textures.clear();

	//Clean up all surfaces
	for (std::pair<SpriteType, SDL_SurfaceDetails*> details : surfaces)
	{
		delete details.second;
	}   surfaces.clear();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}

/// <summary>
/// Binds a sprite type to a surface.
/// </summary>
/// <param name="_file"></param>
/// <param name="_spritetype"></param>
/// <param name="_maxframecount"></param>
void Graphics::Renderer::load_and_bind_surface(const std::string& _file, const Graphics::SpriteType& _spritetype, const uint32_t& _maxframecount = 1)
{
	std::string path = texturepath;
	path.append(_file);

	const char* asset_path = path.c_str();

	assert(surfaces.find(_spritetype) == surfaces.end());
	SDL_Surface* surface = IMG_Load(asset_path);

	//Check if the image got loaded correctly.
	if (surface == nullptr)
	{
		fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
		assert(surface == nullptr);
	}

	//Store the surface details and lock it behind the sprite type.
	surfaces[_spritetype] = new SDL_SurfaceDetails(surface, _maxframecount);
}

void Graphics::Renderer::start_frame()
{
	SDL_RenderClear(renderer);
}

void Graphics::Renderer::debug_render(const Utilities::ivec2& pos, const Utilities::ivec2& size)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaces[SpriteType::TILE_DEBUG]->get_surface());

	const SDL_Rect rect
	{
		pos.x,
		pos.y,
		size.x,
		size.y
	};


	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_DestroyTexture(texture);
}

/// <summary>
/// This function plots the given sprite ontop of the canvas, call endframe when you have build the final frame that you want to render.
/// </summary>
/// <param name="(s)">, Sprite that you want to render ontop of the canvas.</param>
/// <param name="(pos)"></param>
/// <param name="(scale)"></param>
void Graphics::Renderer::plot_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size, const int32_t _gridsize)
{
	if (camera)
	{
		const size_t i = texturehandles[_s.get_handle()];

		int32_t windowSize_w, windowSize_h;
		get_viewport_size(&windowSize_w, &windowSize_h);

		const float viewportWidth = (float)windowSize_w / 2.0f;
		const float viewportHeight = (float)windowSize_h / 2.0f;

		const Utilities::vec2 camPos = camera->get_position();

		SDL_Texture* texture = textures[i];

		// Create a new rect with the given size and transformed position.
		const SDL_Rect destRect
		{
			(int32_t)roundf((_pos.x * (float)_gridsize) - camPos.x + viewportWidth),
			(int32_t)roundf((_pos.y * (float)_gridsize) - camPos.y + viewportHeight),
			(int32_t)roundf(_size.x),
			(int32_t)roundf(_size.y)
		};

		int32_t imageWidth, imageHeight;
		SDL_QueryTexture(texture, NULL, NULL, &imageWidth, &imageHeight);

		const int32_t spriteWidth = imageWidth / _s.get_framecount();
		const int32_t frame = CLAMP(_s.frame, 0, _s.get_framecount());

		const SDL_Rect srcRect
		{
			spriteWidth * frame,
			0,
			spriteWidth,
			imageHeight,
		};


		DEVIOUS_ASSERT(texture != nullptr);

		//Set colors.
		SDL_SetTextureColorMod(texture, _s.color.r, _s.color.b, _s.color.g);
		SDL_SetTextureAlphaMod(texture, _s.color.a);

		SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

		//Reset to default colors after rendering.
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
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

void Graphics::Renderer::destroy_sprite(const Sprite& sprite)
{
	const size_t i = texturehandles[sprite.get_handle()]; //Get index from handle.
	SDL_DestroyTexture(textures[i]);                      //Destroy texture using the index.
	textures[i] = nullptr;				                  //Clear up the index to make it available again.
	texturehandles.erase(sprite.get_handle());		      //Remove availability of the handle.
}

Graphics::Sprite Graphics::Renderer::create_sprite_from_surface(const SpriteType& _spritetype)
{
	auto s_iterator = surfaces.find(_spritetype);
	DEVIOUS_ASSERT(s_iterator != surfaces.end())

	auto t_iterator = std::find(textures.begin(), textures.end(), nullptr);
	DEVIOUS_ASSERT(t_iterator != textures.end()); //Assert if the max capacity of textures have been reached.

	//Create and store texture made from a SDL_Surface
	const int32_t i = (int32_t)std::distance(textures.begin(), t_iterator);

	//Default sprite color.
	const SDL_Color color = { 255, 255, 255, 255 };

	//Generate a handle
	const UUID sprite_handle = UUID::generate(); 

	//Register the sprite handle that refers to the index of the unique texture.
	texturehandles[sprite_handle] = i;

	//Grab the surface details
	const SDL_SurfaceDetails* details = surfaces[_spritetype];

	textures[i] = SDL_CreateTextureFromSurface(renderer, details->get_surface());

	return Sprite { sprite_handle, details->get_framecount(), color };
}

Graphics::Renderer::SDL_SurfaceDetails::SDL_SurfaceDetails(SDL_Surface* _surface, const uint32_t& _framecount) 
	: surface(_surface), framecount(_framecount)
{
}

const uint32_t& Graphics::Renderer::SDL_SurfaceDetails::get_framecount() const
{
	return framecount;
}

SDL_Surface* Graphics::Renderer::SDL_SurfaceDetails::get_surface() const
{
	return surface;
}

Graphics::Renderer::SDL_SurfaceDetails::~SDL_SurfaceDetails()
{
	SDL_FreeSurface(surface);
}
