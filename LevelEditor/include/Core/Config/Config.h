#pragma once
#include "Core/Core.hpp"

#include "Shared/Game/NPCDef.h"

struct Tile;
struct ScenicTile;
struct ObjectTile;
struct NPC;
class Chunk;

enum class e_EntityType;

namespace Graphics 
{
	enum class SpriteType : uint16_t;
}

enum class e_InteractionMode : U8
{
	MODE_DRAG = 0x00,
	MODE_BRUSH,
	MODE_FILL,
	MODE_PICKER,
	MODE_SELECTION,
	MODE_WAND
};

namespace App 
{
	namespace Config 
	{
		constexpr I32      GRIDCELLSIZE = 32;

		const static char* s_FontPath = "data/font/Roboto-Regular.ttf";

		struct TileConfig 
		{
			e_EntityType		 CurrentTileType;
			Graphics::SpriteType SpriteType;
			bool                 bIsWalkable;
			NPCDef               NPCDefinition;
			e_InteractionMode    InteractionMode;
			WeakRef<Chunk>       ChunkClipboard;
		};


		struct SettingsConfig
		{
			Utilities::ivec2 BrushSize = 1;
			bool bShowWalkableTiles    = false;
			bool bRenderChunkVisuals  = true;
		};


		extern TileConfig     TileConfiguration;
		extern SettingsConfig SettingsConfiguration;
	}
}
