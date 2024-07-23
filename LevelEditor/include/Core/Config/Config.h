#pragma once
#include "Core/Core.hpp"

#include "Core/Renderer/SubSprite/SubSprite.hpp"

#include "Shared/Game/NPCDef.h"

class Tile;
class Chunk;

enum class e_SelectedLayer : U8;

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
		constexpr I32 GRIDCELLSIZE = 32;

		const static char* s_FontPath = "data/font/Roboto-Regular.ttf";

		struct TileConfig 
		{
			e_SelectedLayer		 CurrentLayer;
			U32                  VisibleLayerFlags;
			SubSprite            Sprite;
			NPCDef               SelectedNPC;
			float                NPCRespawnTime;
			I32                  NPCid;
			e_InteractionMode    InteractionMode;
			WeakRef<Chunk>       ChunkClipboard;
		};


		struct SettingsConfig
		{
			Utilities::ivec2 BrushSize = Utilities::ivec2(1, 1);
			bool bShowWalkableTiles    = false;
			bool bRenderChunkVisuals   = true;
		};

		extern TileConfig     TileConfiguration;
		extern SettingsConfig SettingsConfiguration;
	}
}
