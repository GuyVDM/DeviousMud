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

void WorldEditor::Place()
{
	using namespace App::Config;

	if(SettingsConfiguration.bFillChunks) 
	{
		FillChunk();
	}
	else 
	{
		PlaceTile();
	}
}

void WorldEditor::Remove() 
{
	using namespace App::Config;

	if (SettingsConfiguration.bFillChunks)
	{
		RemoveChunk();
	}
	else
	{
		RemoveTile();
	}
}

void WorldEditor::PlaceTile()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = GetChunkCoords(m_HoveredGridCell);

	if(!IsValidChunk(chunkCoords))
	{
		m_Chunks[chunkCoords] = std::make_shared<Chunk>();
	}

	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

	const U32 tileIndex = localChunkCoords.x + localChunkCoords.y * SIZE_CHUNK_TILES;

	switch(TileConfiguration.CurrentTileType) 
	{
		case e_EntityType::ENTITY_SCENIC:
		{
			m_Chunks[chunkCoords]->Tiles[tileIndex] = CreateTile(m_HoveredGridCell);
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

		m_Chunks[chunkCoords]->Tiles[tileIndex] = nullptr;
	}
}

const bool WorldEditor::IsValidChunk(const Utilities::ivec2& _chunkCoords) const
{
	return (m_Chunks.find(_chunkCoords) != m_Chunks.end());
}

void WorldEditor::RenderTiles()
{
	//TODO:
	// Only render the chunk if the chunk is on screen and if the chunk is not empty.

	for(auto&[chunkCoords, chunk] : m_Chunks) 
	{
		for(auto& tile : chunk->Tiles) 
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
		static_cast<U32>((mousePos.x / m_Camera->Zoom) + m_Camera->Position.x),
		static_cast<U32>((mousePos.y / m_Camera->Zoom) + m_Camera->Position.y)
	};

	if (worldPos.x < 0)
		worldPos.x -= App::Config::GRIDSIZE - 1;
	if (worldPos.y < 0)
		worldPos.y -= App::Config::GRIDSIZE - 1;

	Utilities::ivec2 gridPos =
	{
		static_cast<U32>(worldPos.x / App::Config::GRIDSIZE),
		static_cast<U32>(worldPos.y / App::Config::GRIDSIZE)
	};

	return gridPos;
}

void WorldEditor::HighlightCurrentTile()
{
	static float elapsedTime = 0.f;

	elapsedTime += App::Config::EditorConfig::GetDT();

	m_HoveredGridCell = GetHoveredGridCell();

	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

	SDL_Rect rect{};
	if(App::Config::SettingsConfiguration.bFillChunks) 
	{
		const Utilities::ivec2 chunkCoords = GetChunkCoords(m_HoveredGridCell);

		rect.x = (chunkCoords.x * SIZE_CHUNK_TILES * App::Config::GRIDSIZE);
		rect.y = (chunkCoords.y * SIZE_CHUNK_TILES * App::Config::GRIDSIZE);
		rect.w = App::Config::GRIDSIZE * SIZE_CHUNK_TILES;
		rect.h = App::Config::GRIDSIZE * SIZE_CHUNK_TILES;
	}
	else 
	{
		rect.x = m_HoveredGridCell.x * App::Config::GRIDSIZE;
		rect.y = m_HoveredGridCell.y * App::Config::GRIDSIZE;
		rect.w = App::Config::GRIDSIZE;
		rect.h = App::Config::GRIDSIZE;
	}

	const Color col = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, elapsedTime)};
	g_globals.Renderer->DrawRect(rect, col, 10);
	
}

void WorldEditor::FillChunk()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = GetChunkCoords(m_HoveredGridCell);

	if(!IsValidChunk(chunkCoords))
	{
		m_Chunks[chunkCoords] = std::make_shared<Chunk>();
	}

	for (U32 x = 0; x < SIZE_CHUNK_TILES; x++)
	{
		for (U32 y = 0; y < SIZE_CHUNK_TILES; y++)
		{
			const Utilities::ivec2 worldCoords =
			{
				chunkCoords.x * SIZE_CHUNK_TILES + x,
				chunkCoords.y * SIZE_CHUNK_TILES + y
			};

			const U32 tileIndex = x + y * SIZE_CHUNK_TILES;
			m_Chunks[chunkCoords]->Tiles[tileIndex] = CreateTile(worldCoords);
		}
	}
}

void WorldEditor::RemoveChunk()
{
	const Utilities::ivec2 chunkCoords = GetChunkCoords(m_HoveredGridCell);
	m_Chunks.erase(chunkCoords);
}

Ref<Tile> WorldEditor::CreateTile(const Utilities::ivec2& _worldCoords) const
{
	Ref<ScenicTile> tile = std::make_shared<ScenicTile>();
	tile->Coords = _worldCoords;
	tile->bIsWalkable = App::Config::TileConfiguration.bIsWalkable;

	return tile;
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

	const Utilities::ivec2 chunkCoords = GetChunkCoords(_worldCoords);

	const Utilities::ivec2 localChunkCoords
	{
		(_worldCoords.x % chunkSize + chunkSize) % chunkSize,
		(_worldCoords.y % chunkSize + chunkSize) % chunkSize
	};

	return localChunkCoords;
}


