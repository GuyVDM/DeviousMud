#pragma once
#include "Shared/Utilities/vec2.hpp"

#include "Core/Rendering/Sprite/Sprite.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Graphics 
{
#pragma region FORWARD_DECLARATIONS
	typedef struct SDL_Renderer SDL_Renderer;
	typedef struct SDL_Window   SDL_Window;
	typedef struct SDL_Surface  SDL_Surface;
	typedef struct SDL_Texture  SDL_Texture;
	typedef struct SDL_Color    SDL_Color;
#pragma endregion

	/// <summary>
	/// Used for the process of plotting sprites ontop of the canvas.
	/// </summary>
    class Renderer final
	{
	public:
		/// <summary>
		/// Nested class to store details about the surface, such as the amount of frames this sprite contains.
		/// This allows for dynamically switching to different images that are saved within the same specified image file.
		/// </summary>
		class SDL_SurfaceDetails final
		{
		public:
			SDL_SurfaceDetails(SDL_Surface* _surface, const uint32_t& _maxframecount);
			~SDL_SurfaceDetails();

			const uint32_t& get_framecount() const;

			SDL_Surface* get_surface() const;

		private:
			uint32_t     framecount;
			SDL_Surface* surface;
		};

	public:
		/// <summary>
		/// Max amount of textures that can get stored realtime.
		/// </summary>
		constexpr static uint32_t MAX_TEXTURE_COUNT = 1000;

		/// <summary>
		/// The size of a cell in pixels.
		/// </summary>
		constexpr static int32_t GRID_CELL_SIZE_PX = 64;

	public:

		static Renderer* create_renderer(const char* _title, const Utilities::ivec2& _c_scale, const std::string& _texture_path);

		void load_and_bind_surface(const std::string& _file, const Graphics::SpriteType& _spritetype, const uint32_t& _framecount);

		void start_frame();

		void plot_frame(const Sprite& s, const Utilities::ivec2& pos, const Utilities::ivec2& size);

		void plot_frame(const Sprite& s, const Utilities::vec2& pos,  const Utilities::vec2& size);

		void end_frame();

		void destroy_sprite(const Sprite& sprite);

		Sprite create_sprite_from_surface(const SpriteType& _spritetype);

		virtual ~Renderer();

	private:
		void free();

	protected:
		Renderer(SDL_Window* _window, SDL_Renderer* _renderer, const std::string& _texture_path);
	
	private:
		std::string texturepath;

		std::vector<SDL_Texture*> textures;

		std::unordered_map<SpriteHandle, int> texturehandles;

		std::unordered_map<SpriteType, SDL_SurfaceDetails*> surfaces;

		SDL_Window* window;

		SDL_Renderer* renderer;
	};


};