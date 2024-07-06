#pragma once

#include "Core/Core.hpp"

#include "Tile/Tile.h"

#include "Shared/Utilities/vec2.hpp"

#include <array>

#include <map>

struct Camera;

constexpr U16 SIZE_CHUNK_TILES = 16;

struct Chunk 
{
	U32 TileCount = 0;
	std::array<Ref<Tile>, SIZE_CHUNK_TILES * SIZE_CHUNK_TILES> Tiles;
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
	void PlaceTile();

	void RemoveTile();

	void FillChunk();

	void RemoveChunk();

	const bool IsValidChunk(const Utilities::ivec2& _chunkCoords) const;

	void RenderTiles();

	void HighlightCurrentTile();

	Ref<Tile> CreateTile(const Utilities::ivec2& _worldCoords) const;

	Utilities::ivec2 GetHoveredGridCell() const;

	Utilities::ivec2 GetChunkCoords(const Utilities::ivec2& _worldCoords) const;

	Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _worldCoords) const;

private:
	Ref<Camera> m_Camera;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;
};