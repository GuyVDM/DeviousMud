#pragma once
#include <cstdint>

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include "Shared/Utilities/EventListener.h"

namespace Graphics 
{
	enum class SpriteType : uint16_t
	{
		NONE = 0,

		// RANKS
		PLAYER_RANK_OWNER = 1,

		//*--------------
		// Not an actual entry.
		// We use this for counting.
		//*
		PLAYER_ICON_COUNT,

		TILE_DEFAULT,
		CROSS,
		PLAYER,

		// HUD
		HUD_BACKDROP,
		HUD_TAB,
		HUD_FRAME,
		HUD_OPTIONS_BOX,
		HUD_CHATBOX,

		// HUD ICONS
		HUD_ICON_PLACEHOLDER,
		HUD_ICON_SKILLS,
		
		//Tab specifics,
		SKILL_BACKGROUND,

		// MISC
		HITSPLAT,

		//Entities
		NPC_GOBLIN,
		NPC_INU,
		
		//*--------------
		// Not an actual entry.
		// We use this for counting.
		//*
		SPRITE_COUNT 
	}; 

	constexpr uint32_t SPRITE_COUNT = static_cast<uint32_t>(SpriteType::SPRITE_COUNT) - 1;

	constexpr uint32_t ICON_COUNT = static_cast<uint32_t>(SpriteType::PLAYER_ICON_COUNT);

	struct SpriteRenderData 
	{
		enum e_SpriteRenderFlags : uint8_t 
		{
			RENDER_WORLDSPACE                = 1 << 0,
			RENDER_FLIP_SPRITE               = 1 << 1,
			RENDER_DESTROY_TEXTURE_AFTER_USE = 1 << 2
		};

		Utilities::vec2 position    = { 0, 0 };
		Utilities::vec2 size        = { 0, 0 };

		SDL_Texture*    texture     = nullptr;
		uint8_t         frameCount  = 0;
		SDL_Color       color       = { 255, 255, 255, 255 };
		uint8_t         frame       = 0;
		uint8_t         renderFlags = 0;

		SpriteRenderData() = default;

		SpriteRenderData(const SpriteRenderData& _other) = default;

		SpriteRenderData& operator=(SpriteRenderData&& _other) noexcept 
		{
			if (this != &_other)
			{
				position	= std::move(_other.position);
				size		= std::move(_other.size);
				texture		= _other.texture;
				frameCount  = _other.frameCount;
				color		= _other.color;
				frame		= _other.frame;
				renderFlags = _other.renderFlags;

				//Clean up.
				_other.texture = nullptr;
				_other.color = { 255, 255, 255, 255 };
				_other.renderFlags = 0;
			}
			return *this;
		}
	};

	typedef class _Sprite 
	{
	public:
		EventListener<uint64_t> on_destroyed;

		SDL_Color color = {255, 255, 255, 255};
		uint8_t   frame = 0;

		const SpriteType get_sprite_type() const;

		const uint8_t    get_framecount() const;

		const DM::Utils::UUID get_uuid() const;

		const float& get_width() const;

		const float& get_height() const;

		const Utilities::vec2& get_dimensions() const;

		_Sprite(const _Sprite& _other);

		_Sprite();

		~_Sprite();

		bool bIsScreenspace = false;

		bool bIsFlipped = false;

		uint8_t zRenderPriority = 0;

	private:
		DM::Utils::UUID m_uuid;
		SpriteType		m_eSpriteType = SpriteType::NONE;
		uint8_t		    m_frameCount = 0;
		Utilities::vec2 m_dimension;

		friend class Renderer;

	}   Sprite;
}