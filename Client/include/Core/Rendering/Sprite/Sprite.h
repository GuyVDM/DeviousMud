#pragma once
#include <cstdint>

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include "Shared/Utilities/EventListener.h"

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
		HUD_FRAME    = 6,
		HUD_OPTIONS_BOX = 7,

		// HUD ICONS
		HUD_ICON_PLACEHOLDER = 8,
		HUD_ICON_SKILLS      = 9,

		//Entities
		NPC_GOBLIN = 10
	}; 

	typedef class _Sprite 
	{
	public:
		EventListener<uint64_t> on_destroyed;

		SDL_Color color = {255, 255, 255, 255};
		uint32_t  frame = 0;

		const SpriteType get_sprite_type() const;

		const uint32_t   get_framecount() const;

		const DM::Utils::UUID get_uuid() const;

		const float& get_width() const;

		const float& get_height() const;

		const Utilities::vec2& get_dimensions() const;

		_Sprite(const _Sprite& _other);

		_Sprite();

		~_Sprite();

		bool bIsFlipped = false;
	private:
		DM::Utils::UUID m_uuid;
		SpriteType		m_eSpriteType = SpriteType::NONE;
		uint32_t		m_frameCount = 0;
		Utilities::vec2 m_dimension;

		friend class Renderer;

	}   Sprite;
}