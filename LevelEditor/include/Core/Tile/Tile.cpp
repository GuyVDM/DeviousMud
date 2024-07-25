#include "precomp.h"

#include "Core/Tile/Tile.h"

#include "Core/Config/Config.h"
#include "Core/Camera/Camera.h"
#include "Core/Globals/Globals.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/WorldEditor/Chunk/Chunk.h"

//Needed for serialization
#include "Core/Tile/Entities/TileEntity/TileEntity.h"
#include "Core/Tile/Entities/NPCEntity/NPCEntity.h"

#include "Shared/Game/NPCDef.h"

Optional<Ref<TileEntity>> Tile::TryGetEntity(const e_SelectedLayer& _layer)
{
	if (m_EntityLayers.find(_layer) == m_EntityLayers.end())
	{
		return std::nullopt;
	}

	return m_EntityLayers[_layer];
}

const bool Tile::IsEmpty() const
{
	return m_EntityLayers.size() == 0;
}

void Tile::InsertLayerEntity(Ref<TileEntity> _tileEntity, const e_SelectedLayer& _layer)
{
	m_EntityLayers[_layer] = _tileEntity;
}

void Tile::RemoveLayerEntity(const e_SelectedLayer& _layer)
{
	m_EntityLayers.erase(_layer);
}

void Tile::Render()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = ChunkCoords * SIZE_CHUNK_TILES;
	const Utilities::ivec2 position    = (LocalChunkCoords + chunkCoords) * App::Config::GRIDCELLSIZE;
	const Utilities::ivec2 size        = Utilities::ivec2(App::Config::GRIDCELLSIZE);
	Color color = { 255, 255, 255, 255 };

	//*---------------------------------------
	// Draw whether tiles are walkable or not.
	//
	if (SettingsConfiguration.bShowWalkableTiles)
	{
		if (!bIsWalkable)
		{
			//Grey out the color if it's not walkable.
			color = { 255, 0, 0, 100 };
		}
	}

	for (const auto&[layer, entity] : m_EntityLayers)
	{
		const U32 bitPosition = static_cast<U32>(layer);

		//Don't render the layer if it's not set to visible.
		if(!U32IsBitSet(TileConfiguration.VisibleLayerFlags, bitPosition)) 
		{
			continue;
		}

		RenderQuery query;
		query.Color      = color;
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
}

Tile::Tile(const Tile* _other)
{
	LocalChunkCoords = _other->LocalChunkCoords;
	ChunkCoords = _other->ChunkCoords;
	bIsWalkable = _other->bIsWalkable;

	for (auto& [layer, tileEntt] : _other->m_EntityLayers)
	{
		m_EntityLayers[layer] = tileEntt->Clone();
	}
}

Tile::Tile()
{
	LocalChunkCoords = Utilities::ivec2(0);
	ChunkCoords = Utilities::ivec2(0);
	bIsWalkable = true;
}
