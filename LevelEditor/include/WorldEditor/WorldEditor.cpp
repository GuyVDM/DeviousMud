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
	HighlightCurrentTile();
	RenderTiles();
}

void WorldEditor::Place()
{
	switch (App::Config::TileConfiguration.InteractionMode)
	{
		case e_InteractionMode::BRUSH:
		{
			PlaceTile();
		}
		break;

		case e_InteractionMode::FILL:
		{
			FillChunk();
		}
		break;

		case e_InteractionMode::TILEPICKER:
		{
			PickTile();
		}
		break;
	}
}

void WorldEditor::Remove() 
{
	switch(App::Config::TileConfiguration.InteractionMode) 
	{
		case e_InteractionMode::FILL:
		{
			RemoveChunk();
		}
		break;

		case e_InteractionMode::BRUSH:
		{
			RemoveTile();

		}
		break;
	}
}

void WorldEditor::CopyChunk()
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

	if (m_Chunks.find(chunkCoords) == m_Chunks.end())
		return;

	App::Config::TileConfiguration.ChunkClipboard = m_Chunks[chunkCoords];
}

void WorldEditor::PasteChunk()
{
	using namespace App::Config;

	if (TileConfiguration.ChunkClipboard.expired())
		return;

	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

	Chunk* chunkClipboard = TileConfiguration.ChunkClipboard.lock().get();

	if (m_Chunks[chunkCoords].get() == chunkClipboard)
		return;

	//*-------------------
	// Duplicate the chunk.
	//
	{
		m_Chunks.erase(chunkCoords);

		Ref<Chunk> chunk = m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);

		for(U32 x = 0; x < SIZE_CHUNK_TILES; x++) 
		{
			for (U32 y = 0; y < SIZE_CHUNK_TILES; y++)
			{
				const U32 tileIndex = x + y * SIZE_CHUNK_TILES;

				if (chunkClipboard->m_Tiles[tileIndex] == nullptr)
					continue;
				
				Ref<ScenicTile> tileToCopy = std::static_pointer_cast<ScenicTile>(chunkClipboard->m_Tiles[tileIndex]);

				Ref<ScenicTile> tile = std::make_shared<ScenicTile>(*tileToCopy);
				chunk->AddTile(tile, Utilities::ivec2(x, y));
			}
		}
	}
}

void WorldEditor::SerializeHoveredChunk()
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

	if (m_Chunks.find(chunkCoords) == m_Chunks.end())
		return;

	m_Chunks[chunkCoords]->Serialize();
}

void WorldEditor::PickTile()
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords      = ToChunkCoords(m_HoveredGridCell);
	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(m_HoveredGridCell);

	if (!IsValidChunk(chunkCoords))
		return;

	const U32 tileIndex = localChunkCoords.x + localChunkCoords.y * SIZE_CHUNK_TILES;

	Ref<Tile> tile = m_Chunks[chunkCoords]->m_Tiles[tileIndex];
	if (tile == nullptr)
		return;

	Ref<ScenicTile> scenicTile = std::static_pointer_cast<ScenicTile>(tile);
	TileConfiguration.SpriteType  = static_cast<Graphics::SpriteType>(scenicTile->SpriteHandle);
	TileConfiguration.bIsWalkable = scenicTile->bIsWalkable;
}

void WorldEditor::RenderChunkBorders()
{
	using namespace App::Config;

	if (!SettingsConfiguration.bRenderChunkOutlines)
	{
		return;
	}


	const Utilities::ivec2 camPos     = g_globals.Camera->Position;

	const Utilities::ivec2 startTile  = ScreenToGridSpace({ 0, 0});
	const Utilities::ivec2 endTile    = ScreenToGridSpace(Utilities::ivec2{ Editor::s_WindowWidth, Editor::s_WindowHeight });

	const Utilities::ivec2 chunkStart = ToChunkCoords(startTile);
	const Utilities::ivec2 chunkEnd   = ToChunkCoords(endTile);

	Ref<Renderer> renderer = g_globals.Renderer;
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

			renderer->DrawRectOutline(rect, { 70, 70, 70, 255 }, 1, 15);
		}
	}
}

void WorldEditor::PlaceTile()
{
	using namespace App::Config;

	for (U32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (U32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
		{
			const Utilities::ivec2 position    = m_HoveredGridCell + Utilities::ivec2(x, y);
			const Utilities::ivec2 chunkCoords = ToChunkCoords(position);

			if (!IsValidChunk(chunkCoords))
			{
				m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
			}

			switch (TileConfiguration.CurrentTileType)
			{
				case e_EntityType::ENTITY_SCENIC:
				{
					m_Chunks[chunkCoords]->AddTile(CreateTile(), ToLocalChunkCoords(position));
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

	for (U32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (U32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
		{
			const Utilities::ivec2 position    = m_HoveredGridCell + Utilities::ivec2{ x, y };
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
	return m_Chunks.find(_chunkCoords) != m_Chunks.end();
}

void WorldEditor::RenderTiles()
{
	for(auto&[chunkCoords, chunk] : m_Chunks) 
	{
		for(auto& tile : chunk->m_Tiles) 
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

	switch(TileConfiguration.InteractionMode) 
	{
		case e_InteractionMode::FILL:
		{
			const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

			rect.x = (chunkCoords.x * SIZE_CHUNK_TILES * GRIDCELLSIZE);
			rect.y = (chunkCoords.y * SIZE_CHUNK_TILES * GRIDCELLSIZE);
			rect.w = GRIDCELLSIZE * SIZE_CHUNK_TILES;
			rect.h = GRIDCELLSIZE * SIZE_CHUNK_TILES;
		}
		break;

		case e_InteractionMode::BRUSH:
		{
			rect.x = m_HoveredGridCell.x * GRIDCELLSIZE;
			rect.y = m_HoveredGridCell.y * GRIDCELLSIZE;
			rect.w = GRIDCELLSIZE * SettingsConfiguration.BrushSize.x;
			rect.h = GRIDCELLSIZE * SettingsConfiguration.BrushSize.y;
		}
		break;

		case e_InteractionMode::TILEPICKER:
		{
			rect.x = m_HoveredGridCell.x * GRIDCELLSIZE;
			rect.y = m_HoveredGridCell.y * GRIDCELLSIZE;
			rect.w = GRIDCELLSIZE;
			rect.h = GRIDCELLSIZE;
		}
		break;
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
		m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
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

			m_Chunks[chunkCoords]->AddTile(CreateTile(), Utilities::ivec2{ x, y });
		}
	}
}

void WorldEditor::RemoveChunk()
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);
	m_Chunks.erase(chunkCoords);
}

Ref<Tile> WorldEditor::CreateTile() const
{
	Ref<ScenicTile> tile   = std::make_shared<ScenicTile>();
	tile->bIsWalkable      = App::Config::TileConfiguration.bIsWalkable;

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

void Chunk::Deserialize(nlohmann::json& _json) 
{

}

void Chunk::Serialize() 
{
	using namespace nlohmann;

	json data;

	//*------------------------
	// Dump the chunk finger print.
	//
	data["Chunk"].push_back
	(
		{
			{ "X", m_ChunkCoords.x },
			{ "Y", m_ChunkCoords.y }
		}
	);

	for(U32 x = 0; x < SIZE_CHUNK_TILES; x++) 
	{
		for(U32 y = 0; y < SIZE_CHUNK_TILES; y++) 
		{
			const U32 tileIndex = x + y * SIZE_CHUNK_TILES;
			
			if (m_Tiles[tileIndex] == nullptr)
				continue;

			Ref<ScenicTile> tile = std::static_pointer_cast<ScenicTile>(m_Tiles[tileIndex]);

			data["Tiles"].push_back
			(
				{
					{"X", x },
					{"Y", y },
					{"SpriteId",   tile->SpriteHandle },
					{"IsWalkable", tile->bIsWalkable  }
				}
			);
		}
	}

	const std::string fileName = "chunk(" + std::to_string(m_ChunkCoords.x) + ',' + std::to_string(m_ChunkCoords.y) + ").json";
	const std::string filePath = "data/chunks/" + fileName;

	std::ofstream outputFile(filePath);

	if (!outputFile.is_open())
	{
		DEVIOUS_ERR("Failed to serialize file: " + filePath);
	}
	else
	{
		DEVIOUS_EVENT("Saved chunk: " << m_ChunkCoords.x << " , "  << m_ChunkCoords.y << " to " << filePath);
		outputFile << data.dump(4);

		outputFile.close();
	}
}