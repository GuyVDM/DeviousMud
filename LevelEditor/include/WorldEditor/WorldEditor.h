#pragma once

#include "Core/Core.hpp"

#include "Tile/Tile.h"

#include "Shared/Utilities/vec2.hpp"

#include "Serialization/JsonSerializable.h"

#include <array>

#include <map>

struct Camera;

constexpr U16 SIZE_CHUNK_TILES = 16;
constexpr U16 TILE_COUNT_CHUNK = SIZE_CHUNK_TILES * SIZE_CHUNK_TILES;

class Chunk : JsonSerializable<Chunk>
{
public:
	IMPLEMENT_SERIALIZABLE;

	inline bool AddTile(Ref<Tile> _tile, const Utilities::ivec2& _localChunkCoords)
	{
		const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

		if(tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if(m_Tiles[tileIndex] == nullptr) 
		{
			m_TileCount++;
		}

		_tile->ChunkCoords      = m_ChunkCoords;
		_tile->LocalChunkCoords = _localChunkCoords;
		m_Tiles[tileIndex] = _tile;
		return true;
	}

	inline bool RemoveTile(const Utilities::ivec2& _localChunkCoords) 
	{
		const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

		if (tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if (m_Tiles[tileIndex] != nullptr)
		{
			m_TileCount--;
		}

		m_Tiles[tileIndex] = nullptr;
		return true;
	}

	inline const bool IsEmpty() const 
	{
		return m_TileCount == 0;
	}

public:
	Chunk(Utilities::ivec2 _chunkCoords) : m_ChunkCoords(_chunkCoords) {};
	virtual ~Chunk() = default;

public:
	Utilities::ivec2 m_ChunkCoords = { 0 };
	U32 m_TileCount = 0;
	std::array<Ref<Tile>,    TILE_COUNT_CHUNK> m_Tiles;
	std::array<Ref<NPCTile>, TILE_COUNT_CHUNK> m_NPCs;
};

class WorldEditor 
{
public:
	void Update();

	void Place();

	void Remove();

	void CopyChunk();

	void PasteChunk();

	void SerializeHoveredChunk();
public:
	WorldEditor();
	virtual ~WorldEditor();

private:
	void PickTile();

	void RenderChunkBorders();

	void PlaceTile();

	void RemoveTile();

	void FillChunk();

	void RemoveChunk();

	const bool IsValidChunk(const Utilities::ivec2& _chunkCoords) const;

	void RenderTiles();

	void HighlightCurrentTile();

	Ref<Tile> CreateTile() const;

	const Utilities::ivec2 GetHoveredGridCell() const;

	const Utilities::ivec2 ToChunkCoords(const Utilities::ivec2& _worldCoords) const;

	const Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _worldCoords) const;

	const Utilities::ivec2 ScreenToGridSpace(const Utilities::ivec2& _screenCoords) const;

private:
	Ref<Camera> m_Camera;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;
};