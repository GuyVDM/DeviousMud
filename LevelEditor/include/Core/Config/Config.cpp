#include "precomp.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/Globals/Globals.h"
#include "Core/Tile/Tile.h"
#include "Core/WorldEditor/TileLayer/TileLayer.h"

App::Config::TileConfig App::Config::TileConfiguration = TileConfig
(
	/* CurrentLayer    */ e_SelectedLayer::LAYER_BACKGROUND,
	/* LayerFlags      */ 0xFFFFFFFF,
	/* Sprite          */ SubSprite(Graphics::SpriteType::TILE_MAP, 0),
	/* NPCDefinition   */ get_npc_definition(0),
	/* NpcRespawnTime  */ 1.0f,
	/* NpcId           */ 0,
	/* InteractionMode */ e_InteractionMode::MODE_BRUSH
	/* ChunkClipboard  */ 
);

App::Config::SettingsConfig App::Config::SettingsConfiguration = SettingsConfig
(
	/* BrushSize            */  Utilities::ivec2(1),
	/* bShowWalkableTiles   */  false,
	/* bRenderChunkOutlines */  true
);