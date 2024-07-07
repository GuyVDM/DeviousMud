#include "precomp.h"

#include "WorldEditor/WorldEditor.h"

#include "Camera/Camera.h"

#include "Config/Config.h"

#include "Renderer/Renderer.h"

#include "Globals/Globals.h"

#include "Editor/Editor.h"

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
	RenderChunkBorders();
	Highlight00();
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

void WorldEditor::RenderChunkBorders()
{
	using namespace App::Config;

	if (!SettingsConfiguration.bRenderChunkOutlines)
	{
		return;
	}

	Ref<Renderer> renderer = g_globals.Renderer;

	Utilities::ivec2 camPos = g_globals.Camera->Position;

	const Utilities::ivec2 startTile = ScreenToGridSpace({ 0, 0});
	const Utilities::ivec2 endTile   = ScreenToGridSpace(Utilities::ivec2{ Editor::s_WindowWidth, Editor::s_WindowHeight });

	const Utilities::ivec2 chunkStart = ToChunkCoords(startTile);
	const Utilities::ivec2 chunkEnd   = ToChunkCoords(endTile);

	constexpr U32 size = GRIDCELLSIZE * SIZE_CHUNK_TILES;

	for (U32 startX = chunkStart.x; startX < chunkEnd.x + 1; startX++)
	{
		for (U32 startY = chunkStart.y; startY < chunkEnd.y + 1; startY++)
		{
			const SDL_Rect rect =
			{
				startX * size,
				startY * size,
				size,
				size				
			};

			renderer->DrawRectOutline(rect, { 100, 100, 100, 255 }, 1, 15);
		}
	}
}

void WorldEditor::Highlight00()
{
	using namespace App::Config;

	const SDL_Rect rect =
	{
		0,
		0,
		GRIDCELLSIZE,
		GRIDCELLSIZE
	};

	g_globals.Renderer->DrawRect(rect, { 0, 0, 120, 120 });
}

void WorldEditor::PlaceTile()
{
	using namespace App::Config;

	for (U32 x = 0; x < SettingsConfiguration.BrushSizeX; x++)
	{
		for (U32 y = 0; y < SettingsConfiguration.BrushSizeY; y++)
		{
			const Utilities::ivec2 position = m_HoveredGridCell + Utilities::ivec2{ x, y };
			const Utilities::ivec2 chunkCoords = ToChunkCoords(position);

			if (!IsValidChunk(chunkCoords))
			{
				m_Chunks[chunkCoords] = std::make_shared<Chunk>();
			}

			const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(position);

			switch (TileConfiguration.CurrentTileType)
			{
				case e_EntityType::ENTITY_SCENIC:
				{
					m_Chunks[chunkCoords]->AddTile(CreateTile(position), localChunkCoords);
				}
				break;

				case e_EntityType::ENTITY_NPC:
				{
				}
				break;
			}
		}
	}
}

void WorldEditor::RemoveTile()
{
	using namespace App::Config;

	for (U32 x = 0; x < SettingsConfiguration.BrushSizeX; x++)
	{
		for (U32 y = 0; y < SettingsConfiguration.BrushSizeY; y++)
		{
			const Utilities::ivec2 position = m_HoveredGridCell + Utilities::ivec2{ x, y };
			const Utilities::ivec2 chunkCoords = ToChunkCoords(position);

			if (m_Chunks.find(chunkCoords) != m_Chunks.end())
			{
				const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(position);

				Ref<Chunk> chunk = m_Chunks[chunkCoords];
				chunk->RemoveTile(localChunkCoords);

				if(chunk->IsEmpty()) 
				{
					m_Chunks.erase(chunkCoords);
				}
			}
		}
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

const Utilities::ivec2 WorldEditor::GetHoveredGridCell() const
{
	Utilities::ivec2 mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	return ScreenToGridSpace(mousePos);
}

void WorldEditor::HighlightCurrentTile()
{
	using namespace App::Config;

	static float elapsedTime = 0.f;

	elapsedTime += App::Config::EditorConfig::GetDT();

	m_HoveredGridCell = GetHoveredGridCell();

	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

	SDL_Rect rect{};
	if(SettingsConfiguration.bFillChunks) 
	{
		const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

		rect.x = (chunkCoords.x * SIZE_CHUNK_TILES * GRIDCELLSIZE);
		rect.y = (chunkCoords.y * SIZE_CHUNK_TILES * GRIDCELLSIZE);
		rect.w = GRIDCELLSIZE * SIZE_CHUNK_TILES;
		rect.h = GRIDCELLSIZE * SIZE_CHUNK_TILES;
	}
	else 
	{
		rect.x = m_HoveredGridCell.x * GRIDCELLSIZE;
		rect.y = m_HoveredGridCell.y * GRIDCELLSIZE;
		rect.w = GRIDCELLSIZE * SettingsConfiguration.BrushSizeX;
		rect.h = GRIDCELLSIZE * SettingsConfiguration.BrushSizeY;
	}

	const Color col = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, elapsedTime)};

	g_globals.Renderer->DrawRect(rect, col, 10);
	g_globals.Renderer->DrawRectOutline(rect, { 120, 120, 0, 120 }, 2, 10);
}

void WorldEditor::FillChunk()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

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

			m_Chunks[chunkCoords]->AddTile(CreateTile(worldCoords), Utilities::ivec2{ x, y });
		}
	}
}

void WorldEditor::RemoveChunk()
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);
	m_Chunks.erase(chunkCoords);
}

Ref<Tile> WorldEditor::CreateTile(const Utilities::ivec2& _worldCoords) const
{
	Ref<ScenicTile> tile = std::make_shared<ScenicTile>();
	tile->Coords = _worldCoords;
	tile->bIsWalkable = App::Config::TileConfiguration.bIsWalkable;

	return tile;
}

const Utilities::ivec2 WorldEditor::ToChunkCoords(const Utilities::ivec2& _worldCoords) const
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

const Utilities::ivec2 WorldEditor::ToLocalChunkCoords(const Utilities::ivec2& _worldCoords) const
{
	const U32 chunkSize = SIZE_CHUNK_TILES; // Size of each chunk in tiles

	const Utilities::ivec2 chunkCoords = ToChunkCoords(_worldCoords);

	const Utilities::ivec2 localChunkCoords
	{
		(_worldCoords.x % chunkSize + chunkSize) % chunkSize,
		(_worldCoords.y % chunkSize + chunkSize) % chunkSize
	};

	return localChunkCoords;
}

const Utilities::ivec2 WorldEditor::ScreenToGridSpace(const Utilities::ivec2& _screenCoords) const
{
	Utilities::ivec2 worldCoords = Renderer::ScreenToWorld(_screenCoords);

	if (worldCoords.x < 0)
		worldCoords.x -= App::Config::GRIDCELLSIZE - 1;
	if (worldCoords.y < 0)
		worldCoords.y -= App::Config::GRIDCELLSIZE - 1;

	return
	{
		static_cast<U32>(worldCoords.x / App::Config::GRIDCELLSIZE),
		static_cast<U32>(worldCoords.y / App::Config::GRIDCELLSIZE)
	};
}


