#pragma once
#include <cstdint>

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

namespace Graphics 
{
	enum class SpriteType : uint16_t
	{
		NONE         = 0,
		TILE_DEFAULT = 1,
		CROSS        = 2,
		PLAYER       = 3,

		// HUD
		HUD_BACKDROP = 4,
		HUD_TAB      = 5,
		HUD_FRAME   =  6,

		// HUD ICONS
		HUD_ICON_PLACEHOLDER = 7,
		HUD_ICON_SKILLS      = 8
	}; 

	typedef class _Sprite 
	{
	public:
		SDL_Color color = {255, 255, 255, 255};
		uint32_t  frame = 0;

		const SpriteType get_sprite_type() const;

		const uint32_t   get_framecount() const;

		const DEVIOUSMUD::RANDOM::UUID get_uuid() const;

		const float& get_width() const;

		const float& get_height() const;

		const Utilities::vec2& get_dimensions() const;

		_Sprite(const _Sprite& _other);

		_Sprite();

	private:
		DEVIOUSMUD::RANDOM::UUID uuid;
		SpriteType		spriteType = SpriteType::NONE;
		uint32_t		frameCount = 0;
		Utilities::vec2 size;

		friend class Renderer;

	}   Sprite;
}