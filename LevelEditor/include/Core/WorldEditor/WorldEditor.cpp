#include "precomp.h"

#include "Core/WorldEditor/WorldEditor.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/Config/Time/Time.hpp"
#include "Core/Editor/Editor.h"
#include "Core/Globals/Globals.h"
#include "Core/Renderer/Renderer.h"

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
	DrawBrush();
	RenderTiles();
	DrawSelection();
}

void WorldEditor::Place()
{
	switch (App::Config::TileConfiguration.InteractionMode)
	{
		case e_InteractionMode::MODE_BRUSH:
		{
			PlaceTile();
		}
		break;

		case e_InteractionMode::MODE_FILL:
		{
			Fill();
		}
		break;

		case e_InteractionMode::MODE_PICKER:
		{
			PickTile();
		}
		break;

		case e_InteractionMode::MODE_SELECTION:
		{
			CreateSelection();
		}
		break;

		case e_InteractionMode::MODE_WAND:
		{
			CreateWandSelection();
		}
		break;
	}
}

void WorldEditor::Remove() 
{
	switch(App::Config::TileConfiguration.InteractionMode) 
	{
		case e_InteractionMode::MODE_BRUSH:
		{
			RemoveTiles();
		}
		break;

		case e_InteractionMode::MODE_FILL:
		{
			RemoveTilesWandSelection();
		}
		break;
	}
}

void WorldEditor::Release()
{
	using namespace App::Config;

	switch(TileConfiguration.InteractionMode) 
	{
		case e_InteractionMode::MODE_SELECTION:
		{
			m_SelectionArgs.bLeftClicked = false;
		}
		break;
	}
}

void WorldEditor::CopyChunk()
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

	if (!IsValidChunk(chunkCoords))
	{
		return;
	}

	App::Config::TileConfiguration.ChunkClipboard = m_Chunks[chunkCoords];
}

void WorldEditor::PasteChunk()
{
	using namespace App::Config;

	if (TileConfiguration.ChunkClipboard.expired())
	{
		return;
	}


	Chunk* chunkClipboard = TileConfiguration.ChunkClipboard.lock().get();

	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);
	if (m_Chunks[chunkCoords].get() == chunkClipboard)
		return;

	m_Chunks.erase(chunkCoords);
	Ref<Chunk> chunk = m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);

	//*-------------------
	// Duplicate the chunk.
	//
	{
		for(U32 x = 0; x < SIZE_CHUNK_TILES; x++) 
		{
			for (U32 y = 0; y < SIZE_CHUNK_TILES; y++)
			{
				const U32 tileIndex = x + y * SIZE_CHUNK_TILES;

				if (chunkClipboard->m_Tiles[tileIndex] == nullptr)
					continue;
				
				const Ref<ScenicTile> tileToCopy = std::static_pointer_cast<ScenicTile>(chunkClipboard->m_Tiles[tileIndex]);

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

Optional<Ref<Tile>> WorldEditor::TryGetTile(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);
	
	if (!IsValidChunk(chunkCoords)) 
	{
		return std::nullopt;
	}

	const U32 tileIndex = Chunk::ToTileIndex(ToLocalChunkCoords(_gridCoords));
	
	if (!m_Chunks[chunkCoords]->IsValidIndex(tileIndex))
	{
		return std::nullopt;
	}

	return m_Chunks[chunkCoords]->m_Tiles[tileIndex];
}

void WorldEditor::CreateSelection()
{
	//Enable Grid Selection.
	m_SelectionArgs.bIsActive = true;

	if (!m_SelectionArgs.bLeftClicked)
	{
		//Clear previous selection
		ClearSelection();

		m_SelectionArgs.bLeftClicked = true;
		m_SelectionArgs.PointA = m_HoveredGridCell;
	}

	m_SelectionArgs.PointB = m_HoveredGridCell;
}

void WorldEditor::ClearSelection()
{
	m_WandSelectedTiles.clear();
	m_SelectionArgs.bIsActive	 = false;
	m_SelectionArgs.bLeftClicked = false;
}

void WorldEditor::CreateWandSelection()
{
	//Clear the previous wand selection.
	m_WandSelectedTiles.clear();

	//*-------------------------
	// We need a selection field to be active
	// Since the magic wand only works within 
	// The selection field.
	if (!m_SelectionArgs.bIsActive)
		return;

	Graphics::SpriteType type = Graphics::SpriteType::NONE;

	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);
	if(IsValidChunk(chunkCoords)) 
	{
		Optional<Ref<Tile>> tile = TryGetTile(m_HoveredGridCell);

		if (tile.has_value())
		{
			const Ref<ScenicTile> scenicTile = std::static_pointer_cast<ScenicTile>(tile.value());
			type = static_cast<Graphics::SpriteType>(scenicTile->SpriteHandle);
		}
	}

	//Recursively draw the wand selection.
	{
		std::vector<Utilities::ivec2> visitedTiles;
		RecursivelyGenWandSelection(type, visitedTiles, m_HoveredGridCell);
	}
}

void WorldEditor::RecursivelyGenWandSelection(const Graphics::SpriteType _toCompare, std::vector<Utilities::ivec2>& _visitedTiles, const Utilities::ivec2& _gridCoords)
{
	//*-------------------------------------------
	// If we already visited this tile, ignore it.
	//
	if (std::find(_visitedTiles.begin(), _visitedTiles.end(), _gridCoords) != _visitedTiles.end())
		return;

	//Mark this as visited.
	_visitedTiles.push_back(_gridCoords);

	//*-------------------------------------------------------------
	// Only deal with it if the point is within the selection field.
	//
	if (!m_SelectionArgs.IsPointOverlapping(_gridCoords))
	{
		return;
	}

	Optional<Ref<Tile>> optTile = TryGetTile(_gridCoords);

	if(optTile.has_value())
	{
		if (_toCompare == Graphics::SpriteType::NONE)
			return;

		Ref<ScenicTile> tile = std::static_pointer_cast<ScenicTile>(optTile.value());
		Graphics::SpriteType tileSprite = static_cast<Graphics::SpriteType>(tile->SpriteHandle);

		if (tileSprite != _toCompare)
			return;
	}
	else 
	{
		if (_toCompare != Graphics::SpriteType::NONE)
			return;
	}

	const std::array<Utilities::ivec2, 4> neighbours =
	{
		_gridCoords + Utilities::ivec2(1, 0),
		_gridCoords - Utilities::ivec2(1, 0),
		_gridCoords + Utilities::ivec2(0, 1),
		_gridCoords - Utilities::ivec2(0, 1)
	};

	for (const Utilities::ivec2& neighbour : neighbours)
	{
		RecursivelyGenWandSelection(_toCompare, _visitedTiles, neighbour);
	}

	m_WandSelectedTiles.push_back(_gridCoords);
}

void WorldEditor::PickTile()
{
	using namespace App::Config;

	Optional<Ref<Tile>> optTile = TryGetTile(m_HoveredGridCell);

	if (!optTile.has_value())
		return;

	const Ref<ScenicTile> tile = std::static_pointer_cast<ScenicTile>(optTile.value());
	TileConfiguration.SpriteType  = static_cast<Graphics::SpriteType>(tile->SpriteHandle);
	TileConfiguration.bIsWalkable = tile->bIsWalkable;
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

	const Ref<Renderer> renderer = g_globals.Renderer;
	constexpr I32 size = GRIDCELLSIZE * SIZE_CHUNK_TILES;

	for (I32 startX = chunkStart.x; startX < chunkEnd.x + 1; startX++)
	{
		for (I32 startY = chunkStart.y; startY < chunkEnd.y + 1; startY++)
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

	for (I32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (I32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
		{
			const Utilities::ivec2 position    = m_HoveredGridCell + Utilities::ivec2(x, y);

			if (m_SelectionArgs.bIsActive)
			{
				if (!m_SelectionArgs.IsPointOverlapping(position))
					continue;
			}
			
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

void WorldEditor::RemoveTiles()
{
	using namespace App::Config;

	for (I32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (I32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
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

void WorldEditor::RemoveTilesWandSelection()
{
	for(const Utilities::ivec2 _tile : m_WandSelectedTiles) 
	{
		const Utilities::ivec2 chunkCoords = ToChunkCoords(_tile);
		
		if (!IsValidChunk(chunkCoords))
			continue;

		const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(_tile);
		const I16 tileIndex = Chunk::ToTileIndex(localChunkCoords);

		m_Chunks[chunkCoords]->m_Tiles[tileIndex] = nullptr;
	}
}

void WorldEditor::Fill()
{
	if (!m_SelectionArgs.bIsActive)
	{
		return;
	}

	if(m_WandSelectedTiles.size() == 0) 
	{
		return;
	}

	for(const Utilities::ivec2& _pos : m_WandSelectedTiles) 
	{
		const Utilities::ivec2 chunkCoords      = ToChunkCoords(_pos);
		const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(_pos);

		if(!IsValidChunk(chunkCoords)) 
		{
			m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
		}

		const I32 tileIndex = Chunk::ToTileIndex(localChunkCoords);
		m_Chunks[chunkCoords]->AddTile(CreateTile(), localChunkCoords);
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

void WorldEditor::DrawBrush()
{
	using namespace App::Config;

	m_HoveredGridCell = GetHoveredGridCell();

	const Color rectCol    = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, Time::GetElapsedTime()) };
	const Color outlineCol = { 120, 120, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, Time::GetElapsedTime()) };

	SDL_Rect rect{};

	switch(TileConfiguration.InteractionMode) 
	{
		case e_InteractionMode::MODE_BRUSH:
		{
			rect.x = m_HoveredGridCell.x * GRIDCELLSIZE;
			rect.y = m_HoveredGridCell.y * GRIDCELLSIZE;
			rect.w = GRIDCELLSIZE * SettingsConfiguration.BrushSize.x;
			rect.h = GRIDCELLSIZE * SettingsConfiguration.BrushSize.y;
		}
		break;

		case e_InteractionMode::MODE_WAND:
		case e_InteractionMode::MODE_PICKER:
		{
			rect.x = m_HoveredGridCell.x * GRIDCELLSIZE;
			rect.y = m_HoveredGridCell.y * GRIDCELLSIZE;
			rect.w = GRIDCELLSIZE;
			rect.h = GRIDCELLSIZE;
		}
		break;
	}

	g_globals.Renderer->DrawRect(rect, rectCol, 10);
	g_globals.Renderer->DrawRectOutline(rect, outlineCol, 2, 10);
}

void WorldEditor::DrawSelection()
{
	if (!m_SelectionArgs.bIsActive)
		return;

	using namespace App::Config;

	const Utilities::ivec2 min =
	{
		std::min(m_SelectionArgs.PointA.x, m_SelectionArgs.PointB.x),
		std::min(m_SelectionArgs.PointA.y, m_SelectionArgs.PointB.y)
	};

	const Utilities::ivec2 max =
	{
		std::max(m_SelectionArgs.PointA.x, m_SelectionArgs.PointB.x),
		std::max(m_SelectionArgs.PointA.y, m_SelectionArgs.PointB.y)
	};

	const Utilities::ivec2 size =
	{
		static_cast<I32>(abs(min.x - max.x)) + 1,
		static_cast<I32>(abs(min.y - max.y)) + 1
	};

	//*----------------------
	// Draw selection field.
	//
	{
		const SDL_Rect rect
		{
			min.x * GRIDCELLSIZE,
			min.y * GRIDCELLSIZE,
			size.x * GRIDCELLSIZE,
			size.y * GRIDCELLSIZE
		};

		constexpr Color outlineCol = { 0, 0, 120, 150 };
		constexpr Color rectCol = { 200, 200, 255, 20 };

		g_globals.Renderer->DrawRect(rect, rectCol, 10);
		g_globals.Renderer->DrawRectOutline(rect, outlineCol, 2, 10);
	}

	//*-------------------------------
	// Draw magic wand selection field.
	//
	{
		constexpr Color rectCol = { 255, 255, 0, 40 };

		for (const Utilities::ivec2& _tile : m_WandSelectedTiles)
		{
			const SDL_Rect rect
			{
				_tile.x * GRIDCELLSIZE,
				_tile.y * GRIDCELLSIZE,
				GRIDCELLSIZE,
				GRIDCELLSIZE
			};

			g_globals.Renderer->DrawRect(rect, rectCol, 10);
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
	const I32 chunkSize = SIZE_CHUNK_TILES; // Size of each chunk in tiles

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
		static_cast<I32>(worldCoords.x / App::Config::GRIDCELLSIZE),
		static_cast<I32>(worldCoords.y / App::Config::GRIDCELLSIZE)
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