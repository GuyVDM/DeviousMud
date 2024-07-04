#include "precomp.h"

#include "WorldEditor/WorldEditor.h"

#include "Camera/Camera.h"

#include "Config/Config.h"

#include "Renderer/Renderer.h"

#include "Globals/Globals.h"

WorldEditor::WorldEditor()
{
	m_Camera = g_globals.Camera;
}

WorldEditor::~WorldEditor()
{
	m_Chunks.clear();
}

void WorldEditor::Update()
{
	HighlightCurrentTile();
	RenderTiles();
}

void WorldEditor::PlaceTile()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords      = GetChunkCoords(m_HoveredGridCell);
	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

	const U32 tileIndex = localChunkCoords.x + localChunkCoords.y * SIZE_CHUNK_TILES;

	switch(TileConfiguration.CurrentTileType) 
	{
		case e_EntityType::ENTITY_SCENIC:
		{
			Ref<ScenicTile> tile = std::make_shared<ScenicTile>();
			tile->Coords = m_HoveredGridCell;
			tile->bIsWalkable = TileConfiguration.bIsWalkable;

			m_Chunks[chunkCoords].Tiles[tileIndex] = tile;
		}
		break;

		case e_EntityType::ENTITY_NPC: 
		{
		}
		break;
	}

}

void WorldEditor::RemoveTile()
{
	const Utilities::ivec2 chunkCoords = GetChunkCoords(m_HoveredGridCell);

	if(m_Chunks.find(chunkCoords) != m_Chunks.end()) 
	{
		const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

		const U32 tileIndex = localChunkCoords.x + localChunkCoords.y * SIZE_CHUNK_TILES;

		m_Chunks[chunkCoords].Tiles[tileIndex] = nullptr;
	}
}

void WorldEditor::RenderTiles()
{
	//TODO:
	// Only render the chunk if the chunk is on screen and if the chunk is not empty.

	for(auto&[chunkCoords, chunk] : m_Chunks) 
	{
		for(auto& tile : chunk.Tiles) 
		{
			if (tile != nullptr)
			{
				tile->Render();
			}
		}
	}
}

Utilities::ivec2 WorldEditor::GetHoveredGridCell() const
{
	Utilities::ivec2 mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	Utilities::ivec2 worldPos
	{
		static_cast<int>((mousePos.x / m_Camera->Zoom) + m_Camera->Position.x),
		static_cast<int>((mousePos.y / m_Camera->Zoom) + m_Camera->Position.y)
	};

	if (worldPos.x < 0)
		worldPos.x -= App::Config::GRIDSIZE - 1;
	if (worldPos.y < 0)
		worldPos.y -= App::Config::GRIDSIZE - 1;

	Utilities::ivec2 gridPos =
	{
		static_cast<int>(worldPos.x / App::Config::GRIDSIZE),
		static_cast<int>(worldPos.y / App::Config::GRIDSIZE)
	};

	return gridPos;
}

void WorldEditor::HighlightCurrentTile()
{
	static float elapsedTime = 0.f;

	elapsedTime += App::Config::EditorConfig::GetDT();

	m_HoveredGridCell = GetHoveredGridCell();

	ToLocalChunkCoords(m_HoveredGridCell);

	const SDL_Rect rect
	{
		m_HoveredGridCell.x * App::Config::GRIDSIZE,
		m_HoveredGridCell.y * App::Config::GRIDSIZE,
		App::Config::GRIDSIZE,
		App::Config::GRIDSIZE
	};

	const Color col = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, elapsedTime)};
	g_globals.Renderer->DrawRect(rect, col, 10);
	
}

Utilities::ivec2 WorldEditor::GetChunkCoords(const Utilities::ivec2& _worldCoords) const
{
	const U32 chunkSize = SIZE_CHUNK_TILES; // Size of each chunk in tiles

	Utilities::ivec2 chunkCoords
	{
		_worldCoords.x / chunkSize,
		_worldCoords.y / chunkSize
	};

	if (_worldCoords.x < 0)
	{
		chunkCoords.x -= 1;
	}
	if (_worldCoords.y < 0) 
	{
		chunkCoords.y -= 1;
	}

	return chunkCoords;
}

Utilities::ivec2 WorldEditor::ToLocalChunkCoords(const Utilities::ivec2& _worldCoords) const
{
	const U32 chunkSize = SIZE_CHUNK_TILES; // Size of each chunk in tiles

	// Get chunk coordinates
	const Utilities::ivec2 chunkCoords = GetChunkCoords(_worldCoords);

	// Calculate local chunk coordinates
	const Utilities::ivec2 localChunkCoords
	{
		(_worldCoords.x % chunkSize + chunkSize) % chunkSize,
		(_worldCoords.y % chunkSize + chunkSize) % chunkSize
	};

	return localChunkCoords;
}


