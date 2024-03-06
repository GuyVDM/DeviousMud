#pragma once
#include <cstdint>

typedef struct SDL_Color SDL_Color;
typedef uint64_t SpriteHandle;

namespace Graphics 
{
	enum class SpriteType : uint16_t
	{
		TILE_DEFAULT = 0,
		PLAYER       = 1,
		TILE_DEBUG   = 2
	}; 

	typedef class _Sprite 
	{
	public:
		SDL_Color color;
		uint32_t frame = 0;

		const uint64_t get_handle() const; 

		const uint64_t get_framecount() const;

	public:
		_Sprite();
		_Sprite(const uint64_t _handle, const uint32_t _framecount, const SDL_Color& _color);
		_Sprite(const _Sprite& _other);

	private:
		uint64_t handle;
		uint32_t framecount;

	}   Sprite;
}