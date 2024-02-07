#include "precomp.h"
#include "Renderer.h"

#include <random>
#include <filesystem>

Graphics::Renderer* Graphics::Renderer::create_renderer(const char* _title, const Utilities::ivec2& _c_scale, const std::string& _texture_path)
{
	SDL_Window* w = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _c_scale.x, _c_scale.y, 0);
	SDL_Renderer* r = SDL_CreateRenderer (w, -1, 0);

	return new Graphics::Renderer(w, r, _texture_path);
}

Graphics::Renderer::Renderer(SDL_Window* _window, SDL_Renderer* _renderer, const std::string& _texture_path) :
	window(_window), renderer(_renderer), texturepath(_texture_path)
{
	textures.resize(MAX_TEXTURE_COUNT);
	std::cout << "[SDL_Renderer] Succesfully created renderer." << std::endl;
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

/// <summary>
/// This function plots the given sprite ontop of the canvas, call endframe when you have build the final frame that you want to render.
/// </summary>
/// <param name="(s)">, Sprite that you want to render ontop of the canvas.</param>
/// <param name="(pos)"></param>
/// <param name="(scale)"></param>
void Graphics::Renderer::plot_frame(const Sprite& s, const Utilities::ivec2& pos, const Utilities::ivec2& size)
{
	const size_t i = texturehandles[s.get_handle()];

	const SDL_Rect r
	{
		pos.x, pos.y,
		size.x, size.y
	};

	SDL_Texture* texture = textures[i];

	assert(texture != nullptr);

	//Set colors.
	SDL_SetTextureColorMod(texture, s.color.r, s.color.b, s.color.g);
	SDL_SetTextureAlphaMod(texture, s.color.a);

	SDL_RenderCopy(renderer, texture, NULL, &r);

	//Reset to default colors after rendering.
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

/// <summary>
/// This function plots the given sprite ontop of the canvas, call endframe when you have build the final frame that you want to render.
/// </summary>
/// <param name="(s)">, Sprite that you want to render ontop of the canvas.</param>
/// <param name="(pos)"></param>
/// <param name="(scale)"></param>
void Graphics::Renderer::plot_frame(const Sprite& s, const Utilities::vec2& pos, const Utilities::vec2& size)
{
	const Utilities::ivec2 ipos
	{
		static_cast<int32_t>(std::floor(pos.x)),
		static_cast<int32_t>(std::floor(pos.y))
	};

	const Utilities::ivec2 isize
	{
		static_cast<int32_t>(std::floor(size.x)),
		static_cast<int32_t>(std::floor(size.y))
	};

	plot_frame(s, ipos, isize);
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
	texturehandles.erase(sprite.get_handle());		          //Remove availability of the handle.
}

Graphics::Sprite Graphics::Renderer::create_sprite_from_surface(const SpriteType& _spritetype)
{
	auto s_iterator = surfaces.find(_spritetype);
	if(s_iterator == surfaces.end()) 
	{
		fprintf(stderr, "Could not find the sprite specified.");
		return Sprite
		{
		};
	}

	auto t_iterator = std::find(textures.begin(), textures.end(), nullptr);
	assert(t_iterator != textures.end()); //Assert if the max capacity of textures have been reached.

	//Create and store texture made from a SDL_Surface
	int i = std::distance(textures.begin(), t_iterator);

	//Default sprite color.
	const SDL_Color color = { 255, 255, 255, 255 };

	//Generate a handle based on normal distribution
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;
	const SpriteHandle sprite_handle = dis(gen); 

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
