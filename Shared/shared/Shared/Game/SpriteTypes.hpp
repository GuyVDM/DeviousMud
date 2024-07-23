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

		TILE_MAP,
		STRUCTURES_MAP,

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
		std::string  Path;
		SpriteType   Type;
		uint32_t     Rows;
		uint32_t     Columns;

		SpriteArgs(std::string _path, Graphics::SpriteType _type, uint32_t _rows = 1, uint32_t _columns = 1)
			: Path(_path), Type(_type), Rows(_rows), Columns(_columns)
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
					{ "player/Player_Sheet.png",            SpriteType::PLAYER,                1, 28 },
					{ "player/rank_owner.png",              SpriteType::PLAYER_RANK_OWNER            },
					{ "Tile.png",                           SpriteType::TILE_DEFAULT                 },
					{ "Cross.png",                          SpriteType::CROSS,                 1, 14 },

					//Load in all HUD elements.
					{ "hud/backdrop.png",                   SpriteType::HUD_BACKDROP                 },
					{ "hud/tabs/tab.png",                   SpriteType::HUD_TAB                      },
					{ "hud/frame.png",                      SpriteType::HUD_FRAME                    },
					{ "hud/box.png",                        SpriteType::HUD_OPTIONS_BOX              },
					{ "hud/chatbox.png",                    SpriteType::HUD_CHATBOX                  },

					{ "hud/skill/skillcontainer.png",       SpriteType::SKILL_BACKGROUND             },

					// Load in misc.
				    { "hitsplat.png",                       SpriteType::HITSPLAT                     },

				    //Load in all icons.
				    { "hud/tabs/icons/default.png",         SpriteType::HUD_ICON_PLACEHOLDER         },
				    { "hud/tabs/icons/skills.png",          SpriteType::HUD_ICON_SKILLS              },
				    { "hud/tabs/icons/combat.png",          SpriteType::HUD_ICON_COMBAT              },
				    { "hud/tabs/icons/inventory.png",       SpriteType::HUD_ICON_INVENTORY           },

				    //Load in all entities
				    { "entity/0.png",                       SpriteType::NPC_GOBLIN,            1, 50 },
				    { "entity/1.png",                       SpriteType::NPC_INU,               1, 18 },
					{ "world/tile/Tilemap.png",             SpriteType::TILE_MAP,              3, 5  },
					{ "world/structure/StructureMap.png",   SpriteType::STRUCTURES_MAP,        3, 3  }
			};

			return map;
		}
	};
}