#pragma once

#include "Core/Core.hpp"

#include "Tile/Tile.h"

#include "Shared/Utilities/vec2.hpp"

#include <array>

#include <map>

struct Camera;

constexpr U16 SIZE_CHUNK_TILES = 32;

struct Chunk 
{
	std::array<Ref<Tile>, SIZE_CHUNK_TILES> Tiles;

	Utilities::ivec2 ChunkCoordinates;
};

class WorldEditor 
{
public:
	WorldEditor();
	virtual ~WorldEditor() = default;

	void Update();

	void PlaceTile();

private:
	void CheckNewHighlight();

	void HighlightCurrentTile();

private:
	Ref<Camera> m_Camera;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	std::map<Utilities::ivec2, Chunk> m_Chunks;
};