#include "precomp.h"
#include "Core/WorldEditor/Chunk/Chunk.h"
#include "Core/Config/Config.h"

#include "Core/Tile/Tile.h"

const U32 Chunk::ToTileIndex(const Utilities::ivec2& _localChunkCoords)
{
	return _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;
}

bool Chunk::AddToTile(const Utilities::ivec2& _localChunkCoords, Ref<TileEntity> _tileEntity, const e_SelectedLayer& _layer)
{
	const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

	if (tileIndex < 0 && tileIndex >= TILE_COUNT_CHUNK)
	{
		return false;
	}

	Ref<Tile>& tilePtr = m_Tiles[tileIndex];

	if(!tilePtr)
	{
		Tile tile = {};
		tile.bIsWalkable      = App::Config::TileConfiguration.bIsWalkable;
		tile.ChunkCoords      = m_ChunkCoords;
		tile.LocalChunkCoords = _localChunkCoords;

		tilePtr = std::make_shared<Tile>(&tile);
		m_TileCount++;
	}

	const I32 spriteIndex = static_cast<I32>(_tileEntity->SpriteType);

	tilePtr->InsertLayerEntity(_tileEntity, _layer);
	return true;
}

bool Chunk::AddTile(Ref<Tile> _tile)
{
	if(!_tile) 
	{
		return false;
	}

	_tile->ChunkCoords = m_ChunkCoords;

	const U32 index = ToTileIndex(_tile->LocalChunkCoords);
	if(!m_Tiles[index]) 
	{
		m_TileCount++;
	}

	m_Tiles[index] = _tile;
	return true;
}

bool Chunk::RemoveFromTile(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer)
{
	const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

	if (tileIndex < 0 && tileIndex >= TILE_COUNT_CHUNK)
	{
		return false;
	}

	Ref<Tile>& tile = m_Tiles[tileIndex];
	if(!tile) 
	{
		return false;
	}

	tile->RemoveLayerEntity(_layer);

	if(tile->IsEmpty()) 
	{
		m_Tiles[tileIndex] = nullptr;
		m_TileCount--;
	}

	return true;
}

Optional<Ref<TileEntity>> Chunk::DetachFromTile(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer)
{
	Optional<Ref<TileEntity>> optEntt = TryGetTileEntity(_localChunkCoords, _layer);

	if(optEntt.has_value())
	{
		Ref<TileEntity> entt = optEntt.value();
		RemoveFromTile(_localChunkCoords, _layer);

		return entt;
	}

	return std::nullopt;
}

const Optional<Ref<TileEntity>> Chunk::TryGetTileEntity(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer)
{
	const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

	if (tileIndex < 0 && tileIndex >= TILE_COUNT_CHUNK)
	{
		return std::nullopt;
	}

	Ref<Tile>& tile = m_Tiles[tileIndex];
	if (!tile)
	{
		return std::nullopt;
	}

	Optional<Ref<TileEntity>> optTileEntt = tile->TryGetEntity(_layer);
	if(!optTileEntt.has_value()) 
	{
		return std::nullopt;
	}

	return optTileEntt.value();
}

const bool Chunk::IsValidIndex(const U32& _index) const
{
	if (_index < 0 || _index >= TILE_COUNT_CHUNK)
	{
		return false;
	}

	return m_Tiles[_index] != nullptr;
}

const bool Chunk::IsEmpty() const
{
	return m_TileCount == 0;
}

const Utilities::ivec2 Chunk::ToChunkCoords(const Utilities::ivec2& _gridCoords)
{
	return  Utilities::ivec2
	{
		(_gridCoords.x < 0) ? ((_gridCoords.x + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.x / SIZE_CHUNK_TILES),
		(_gridCoords.y < 0) ? ((_gridCoords.y + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.y / SIZE_CHUNK_TILES)
	};;
}

const Utilities::ivec2 Chunk::ToLocalChunkCoords(const Utilities::ivec2& _gridCoords)
{
	return Utilities::ivec2
	{
		(_gridCoords.x % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES,
		(_gridCoords.y % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES
	};
}

void Chunk::Deserialize(nlohmann::json& _json) 
{
};

void Chunk::Serialize() 
{

};
