#include "precomp.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/Globals/Globals.h"
#include "Core/Tile/Tile.h"

App::Config::TileConfig App::Config::TileConfiguration = TileConfig
(
	/* CurrentTileType */ e_EntityType::ENTITY_DEFAULT,
	/* Sprite          */ SubSprite(Graphics::SpriteType::TILE_MAP, 0),
	/* isWalkable      */ true,
	/* NPCDefinition   */ get_npc_definition(0),
	/* InteractionMode */ e_InteractionMode::MODE_BRUSH
	/* ChunkClipboard  */ 
);

App::Config::SettingsConfig App::Config::SettingsConfiguration = SettingsConfig
(
	/* BrushSize            */  Utilities::ivec2(1),
	/* bShowWalkableTiles   */  false,
	/* bRenderChunkOutlines */  true
);