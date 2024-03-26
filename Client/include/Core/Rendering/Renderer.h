#pragma once
#include "Shared/Utilities/vec2.hpp"

#include "Shared/Utilities/EventListener.h"

#include "Core/Rendering/Sprite/Sprite.h"


#include <string>

#include <unordered_map>

#include <vector>

#pragma region FORWARD_DECLARATIONS
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window   SDL_Window;
typedef struct SDL_Surface  SDL_Surface;
typedef struct SDL_Texture  SDL_Texture;
typedef struct SDL_Color    SDL_Color;
class Camera;
#pragma endregion

namespace Graphics 
{
	/// <summary>
	/// Used for the process of plotting sprites ontop of the canvas.
	/// </summary>
    class Renderer final
	{
	public:
		EventListener<Utilities::vec2> on_viewport_size_changed;

		/// <summary>
		/// Nested class to store details about the surface, such as the amount of frames this sprite contains.
		/// This allows for dynamically switching to different images that are saved within the same specified image file.
		/// </summary>
		class SDL_SpriteDetails final
		{
		public:
			SDL_SpriteDetails(SDL_Surface* _surface, SDL_Texture* _texture, const uint32_t& _maxframecount);
			virtual ~SDL_SpriteDetails();

			const uint32_t& get_framecount() const;

			SDL_Surface* get_surface() const;

			SDL_Texture* get_texture();


		private:
			uint32_t     framecount;
			SDL_Surface* surface;
			SDL_Texture* texture;
		};

	public:
		/// <summary>
		/// Max amount of textures that can get stored realtime.
		/// </summary>
		constexpr static uint32_t MAX_TEXTURE_COUNT = 1000;

		/// <summary>
		/// The size of a cell in pixels.
		/// </summary>
		constexpr static int32_t GRID_CELL_PX_SIZE = 64;

	public:

		static Renderer* create_renderer(const char* _title, const Utilities::ivec2& _c_scale, const std::string& _texture_path);

		void set_camera(std::shared_ptr<Camera>& _camera);

		std::shared_ptr<Camera>& get_camera();

		void load_and_bind_surface(const std::string& _file, const Graphics::SpriteType& _spritetype, const uint32_t& _framecount = 1);

		void start_frame();

		void plot_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size, const int32_t _gridsize = GRID_CELL_PX_SIZE);

		void plot_raw_frame(const Sprite& _s, const Utilities::vec2& _pos, const Utilities::vec2& _size);

		void get_viewport_size(int32_t* _w, int32_t* _h);

		void end_frame();

		Sprite get_sprite(const SpriteType& _spritetype);

		virtual ~Renderer();

	private:
		void free();

	protected:
		Renderer(SDL_Window* _window, SDL_Renderer* _renderer, const std::string& _texture_path);
	
	private:
		std::shared_ptr<Camera> camera;

		std::string texturepath;

		std::unordered_map<SpriteType, SDL_SpriteDetails*> sprites;

		SDL_Window* window;

		SDL_Renderer* renderer;
	};


};