#pragma once

typedef unsigned short uint16_t;

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
		HUD_ICON_COMBAT,
		HUD_ICON_INVENTORY,

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

	constexpr inline uint32_t SPRITE_COUNT = static_cast<uint32_t>(SpriteType::SPRITE_COUNT) - 1;

	constexpr inline uint32_t ICON_COUNT = static_cast<uint32_t>(SpriteType::PLAYER_ICON_COUNT);

	typedef int int32_t;

	struct SpriteArgs
	{
		std::string Path;
		SpriteType  Type;
		int32_t     FrameCount;

		SpriteArgs(std::string _path, Graphics::SpriteType _type, int32_t _frameCount = 1) : Path(_path), Type(_type), FrameCount(_frameCount)
		{
		}
	};

	struct SpriteConfig
	{
		using SpriteMap = std::vector<SpriteArgs>;

		static const SpriteMap spriteMap()
		{
			using namespace Graphics;

			static const SpriteMap map =
			{
					{ "player/Player_Sheet.png",            SpriteType::PLAYER,                28 },
					{ "player/rank_owner.png",              SpriteType::PLAYER_RANK_OWNER         },
					{ "Tile.png",                           SpriteType::TILE_DEFAULT              },
					{ "Cross.png",                          SpriteType::CROSS,                 14 },

					//Load in all HUD elements.
					{ "hud/backdrop.png",                   SpriteType::HUD_BACKDROP              },
					{ "hud/tabs/tab.png",                   SpriteType::HUD_TAB                   },
					{ "hud/frame.png",                      SpriteType::HUD_FRAME                 },
					{ "hud/box.png",                        SpriteType::HUD_OPTIONS_BOX           },
					{ "hud/chatbox.png",                    SpriteType::HUD_CHATBOX               },

					{ "hud/skill/skillcontainer.png",       SpriteType::SKILL_BACKGROUND          },

					// Load in misc.
				    { "hitsplat.png",                       SpriteType::HITSPLAT                  },

				    //Load in all icons.
				    { "hud/tabs/icons/default.png",         SpriteType::HUD_ICON_PLACEHOLDER      },
				    { "hud/tabs/icons/skills.png",          SpriteType::HUD_ICON_SKILLS           },
				    { "hud/tabs/icons/combat.png",          SpriteType::HUD_ICON_COMBAT           },
				    { "hud/tabs/icons/inventory.png",       SpriteType::HUD_ICON_INVENTORY        },

				    //Load in all entities
				    { "entity/0.png",                       SpriteType::NPC_GOBLIN,            50 },
				    { "entity/1.png",                       SpriteType::NPC_INU,               18 }

			};

			return map;
		}
	};
}