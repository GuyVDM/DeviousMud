#pragma once

#include "Core/Core.hpp"

#include "Tile/Tile.h"

#include "Shared/Utilities/vec2.hpp"

#include <array>

#include <map>

struct Camera;

constexpr U16 SIZE_CHUNK_TILES = 16;
constexpr U16 TILE_COUNT_CHUNK = SIZE_CHUNK_TILES * SIZE_CHUNK_TILES;

class Chunk 
{
public:
	inline bool AddTile(Ref<Tile> _tile, const Utilities::ivec2& _localChunkCoords)
	{
		const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

		if(tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if(Tiles[tileIndex] == nullptr) 
		{
			TileCount++;
		}

		Tiles[tileIndex] = _tile;
		return true;
	}

	inline bool RemoveTile(const Utilities::ivec2& _localChunkCoords) 
	{
		const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

		if (tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if (Tiles[tileIndex] != nullptr)
		{
			TileCount--;
		}

		Tiles[tileIndex] = nullptr;
		return true;
	}

	inline const bool IsEmpty() const 
	{
		return TileCount == 0;
	}

public:
	Chunk() = default;
	virtual ~Chunk() = default;

public:
	U32 TileCount = 0;
	std::array<Ref<Tile>, TILE_COUNT_CHUNK> Tiles;
};

class WorldEditor 
{
public:
	void Update();

	void Place();

	void Remove();

public:
	WorldEditor();
	virtual ~WorldEditor();

private:
	void RenderChunkBorders();

	void Highlight00();

	void PlaceTile();

	void RemoveTile();

	void FillChunk();

	void RemoveChunk();

	const bool IsValidChunk(const Utilities::ivec2& _chunkCoords) const;

	void RenderTiles();

	void HighlightCurrentTile();

	Ref<Tile> CreateTile(const Utilities::ivec2& _worldCoords) const;

	const Utilities::ivec2 GetHoveredGridCell() const;

	const Utilities::ivec2 ToChunkCoords(const Utilities::ivec2& _worldCoords) const;

	const Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _worldCoords) const;

	const Utilities::ivec2 ScreenToGridSpace(const Utilities::ivec2& _screenCoords) const;

private:
	Ref<Camera> m_Camera;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;
};