#include "precomp.h"

#include "Core/Tile/Tile.h"

#include "Core/Config/Config.h"
#include "Core/Globals/Globals.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/WorldEditor/Chunk/Chunk.h"

#include "Shared/Game/NPCDef.h"

void Tile::Render()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = ChunkCoords * SIZE_CHUNK_TILES;
	const Utilities::ivec2 position    = (LocalChunkCoords + chunkCoords) * App::Config::GRIDCELLSIZE;
	const Utilities::ivec2 size        = Utilities::ivec2(App::Config::GRIDCELLSIZE);

	for (const auto&[layer, entity] : m_EntityLayers)
	{
		const U32 bitPosition = static_cast<U32>(layer);

		//Don't render the layer if it's not set to visible.
		if(!U32IsBitSet(TileConfiguration.VisibleLayerFlags, bitPosition)) 
		{
			continue;
		}

		RenderQuery query;
		query.Color      = { 255,255,255,255 };
		query.Frame      = entity->Frame;
		query.SpriteType = entity->SpriteType;
		query.Position   = position;
		query.Size       = size;

		const U8 zOrder = static_cast<U8>(layer);
		g_globals.Renderer->Render(query, zOrder);
	}

	const SDL_Rect rect
	{
		position.x,
		position.y,
		size.x,
		size.y
	};

	//*---------------------------------------
	// Draw whether tiles are walkable or not.
	//
	if (App::Config::SettingsConfiguration.bShowWalkableTiles)
	{
		const Color color = bIsWalkable ? Color(0, 255, 0, 70) 
			                            : Color(255, 0, 0, 70);

		g_globals.Renderer->DrawRect(rect, color);
	}
}