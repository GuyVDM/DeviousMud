#include "precomp.h"

#include "Core/WorldEditor/WorldEditor.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/Config/Time/Time.hpp"
#include "Core/Editor/Editor.h"
#include "Core/Globals/Globals.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/Renderer.h"

#include "Shared/Game/SpriteTypes.hpp"


WorldEditor::WorldEditor()
{
	m_Camera = g_globals.Camera;
}

void WorldEditor::Update()
{
	RenderChunkVisuals();
	DrawBrush();
	RenderTiles();
	DrawSelection();
}

void WorldEditor::MoveTileTo(Ref<Tile> _tile, Utilities::ivec2 _gridCoords)
{
	//*---------------------------------------------
	//Remove tile from previous chunk if applicable.
	//
	{
		const Utilities::ivec2 chunkCoords = _tile->ChunkCoords;
		if (IsValidChunk(chunkCoords))
		{
			m_Chunks[chunkCoords]->RemoveTile(ToLocalChunkCoords(_gridCoords));
		}
	}

	//*---------------------------------------------
	// Move the tile over to it's new chunk
	// If the chunk doesn't exist yet, we create it.
	{
		const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);

		Ref<Chunk> chunk;
		if(!IsValidChunk(chunkCoords)) 
		{
			chunk = m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
		}
		else 
		{
			chunk = m_Chunks[chunkCoords];
		}

		_tile->ChunkCoords      = chunkCoords;
		_tile->LocalChunkCoords = ToLocalChunkCoords(_gridCoords);
		
		chunk->AddTile(_tile);
	}
}

void WorldEditor::Place()
{
	switch (App::Config::TileConfiguration.InteractionMode)
	{
		case e_InteractionMode::MODE_BRUSH:
		{
			PlaceBrushTiles();
		}
		break;

		case e_InteractionMode::MODE_FILL:
		{
			//Just doing it once per input is fine.
			if (g_Input->GetMouseDown(e_MouseButton::BUTTON_LEFT))
			{
				Fill();
			}
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
			//Just doing it once per input is fine.
			if(g_Input->GetMouseDown(e_MouseButton::BUTTON_LEFT)) 
			{
				CreateWandSelection();
			}
		}
		break;

		case e_InteractionMode::MODE_DRAG:
		{
			DragSelectedTiles();
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
			RemoveBrushTiles();
		}
		break;

		case e_InteractionMode::MODE_FILL:
		{
			//Just doing it once per input is fine.
			if (g_Input->GetMouseDown(e_MouseButton::BUTTON_RIGHT))
			{
				RemoveTilesSelection();
			}
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
			m_SelectionArgs.bIsInteracting = false;
		}
		break;

		case e_InteractionMode::MODE_DRAG:
		{
			TryPlaceSelectedTiles();
		}
		break;
	}
}

void WorldEditor::CloneChunk()
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

	const Utilities::ivec2 chunkCoords = ToChunkCoords(m_HoveredGridCell);

	Ref<Chunk> cbChunk = TileConfiguration.ChunkClipboard.lock();

	if (m_Chunks[chunkCoords] == cbChunk)
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
				const U32 i = Chunk::ToTileIndex(Utilities::ivec2(x, y));

				if (cbChunk->m_Tiles[i] == nullptr)
					continue;
				
				const Ref<Tile> tileCopy = std::make_shared<Tile>(cbChunk->m_Tiles[i].get());
				tileCopy->ChunkCoords    = chunkCoords;
				chunk->AddTile(tileCopy);
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

Optional<Ref<Tile>> WorldEditor::TryDetachTile(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);

	if (!IsValidChunk(chunkCoords))
	{
		return std::nullopt;
	}

	const Utilities::ivec2 localChunkCoords = ToLocalChunkCoords(_gridCoords);
	const U32 tileIndex = Chunk::ToTileIndex(localChunkCoords);

	Ref<Chunk> chunk = m_Chunks[chunkCoords];
	if (!chunk->IsValidIndex(tileIndex))
	{
		return std::nullopt;
	}

	Ref<Tile> tile = chunk->m_Tiles[tileIndex];
	chunk->RemoveTile(localChunkCoords);

	return tile;
}

Optional<Ref<Chunk>> WorldEditor::TryGetChunk(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);

	if(IsValidChunk(chunkCoords)) 
	{
		return m_Chunks[chunkCoords];
	}

	return std::nullopt;
}

void WorldEditor::PlaceTile(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);

	if (!IsValidChunk(chunkCoords)) 
	{
		m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
	}

	Ref<Tile> tile;
	switch(App::Config::TileConfiguration.CurrentTileType) 
	{
		case e_EntityType::ENTITY_DEFAULT:
		{
			tile = std::make_shared<Tile>();
		}
		break;
	}

	tile->sprite           = App::Config::TileConfiguration.SpriteType;
	tile->bIsWalkable      = App::Config::TileConfiguration.bIsWalkable;
	tile->LocalChunkCoords = ToLocalChunkCoords(_gridCoords);
	tile->ChunkCoords      = ToChunkCoords(_gridCoords);

	m_Chunks[chunkCoords]->AddTile(tile);
}

void WorldEditor::RemoveTile(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = ToChunkCoords(_gridCoords);

	Optional<Ref<Chunk>> optChunk = TryGetChunk(_gridCoords);

	if(optChunk.has_value()) 
	{
		Ref<Chunk> chunk = optChunk.value();
		chunk->RemoveTile(ToLocalChunkCoords(_gridCoords));
		
		//Destroy the chunk if there's no tiles left.
		if(chunk->IsEmpty()) 
		{
			m_Chunks.erase(chunkCoords);
		}
	}
}

void WorldEditor::DragSelectedTiles()
{
	if (!m_SelectionArgs.bIsActive)
	{
		return;
	}

	if (!m_SelectionArgs.bIsDragging)
	{
		m_SelectionArgs.bIsDragging  = true;
		m_SelectionArgs.startDragPos = m_HoveredGridCell;
		m_SelectionArgs.startPointA  = m_SelectionArgs.PointA;
		m_SelectionArgs.startPointB  = m_SelectionArgs.PointB;
		m_SelectionArgs.StartWandSelectedTiles = m_SelectionArgs.WandSelectedTiles;
		
		//Collect tiles from wand
		for (const Utilities::ivec2& selectionPos : m_SelectionArgs.GetAffectedTiles())
		{
			Optional<Ref<Tile>> optTile = TryDetachTile(selectionPos);

			if (!optTile.has_value())
			{
				continue;
			}

			DragArgs args;
			args.Tile  = optTile.value();
			args.StartGridPos = (args.Tile->ChunkCoords * SIZE_CHUNK_TILES) + args.Tile->LocalChunkCoords;
			m_SelectionArgs.SelectedTiles.push_back(args);
		}
	}

	m_SelectionArgs.MoveSelectionRelativeTo(m_HoveredGridCell);
}

void WorldEditor::TryPlaceSelectedTiles()
{
	for(const DragArgs& _args : m_SelectionArgs.SelectedTiles) 
	{
		const Utilities::ivec2 gridPos = (_args.Tile->ChunkCoords * SIZE_CHUNK_TILES) + _args.Tile->LocalChunkCoords;
		MoveTileTo(_args.Tile, gridPos);
	}

	m_SelectionArgs.SelectedTiles.clear();
	m_SelectionArgs.bIsDragging = false;
}

void WorldEditor::CreateSelection()
{
	//Enable Grid Selection.
	m_SelectionArgs.bIsActive = true;

	if (!m_SelectionArgs.bIsInteracting)
	{
		//Clear previous selection
		ClearSelection();

		m_SelectionArgs.bIsInteracting = true;
		m_SelectionArgs.PointA = m_HoveredGridCell;
	}

	m_SelectionArgs.PointB = m_HoveredGridCell;
}

void WorldEditor::ClearSelection()
{
	TryPlaceSelectedTiles();

	m_SelectionArgs.SelectedTiles.clear();
	m_SelectionArgs.WandSelectedTiles.clear();
	m_SelectionArgs.startDragPos = { 0 };
	m_SelectionArgs.startPointA  = { 0 };
	m_SelectionArgs.startPointB  = { 0 };
	m_SelectionArgs.bIsActive	 = false;
	m_SelectionArgs.bIsInteracting = false;
	m_SelectionArgs.bIsDragging  = false;
}

const bool WorldEditor::IsSelectionActive() const
{
	return m_SelectionArgs.bIsActive;
}

const bool WorldEditor::IsHoveringOverActiveChunk() const
{
	return IsValidChunk(ToChunkCoords(m_HoveredGridCell));
}

void WorldEditor::CreateWandSelection()
{
	//Clear the previous wand selection.

	const bool bCtrlPressed = g_Input->GetKey(SDLK_RCTRL) || g_Input->GetKey(SDLK_LCTRL);
	const bool bAltPressed  = g_Input->GetKey(SDLK_RALT) || g_Input->GetKey(SDLK_LALT);


	const bool bNoReset = bCtrlPressed || bAltPressed;
	if (!bNoReset)
	{
		m_SelectionArgs.WandSelectedTiles.clear();
	}

	//*-------------------------
	// We need a selection field to be active
	// Since the magic wand only works within 
	// The selection field.
	if (!m_SelectionArgs.bIsActive)
	{
		return;
	}

	Graphics::SpriteType type = Graphics::SpriteType::NONE;

	if(IsValidChunk(ToChunkCoords(m_HoveredGridCell)))
	{
		Optional<Ref<Tile>> tile = TryGetTile(m_HoveredGridCell);

		if (tile.has_value())
		{
			type = tile.value()->sprite;
		}
	}

	//Recursively draw the wand selection.
	{
		std::vector<Utilities::ivec2> visitedTiles;
		RecursivelyGenWandSelection(type, visitedTiles, m_HoveredGridCell, bAltPressed);
	}
}

void WorldEditor::RecursivelyGenWandSelection(const Graphics::SpriteType _toCompare, std::vector<Utilities::ivec2>& _visitedTiles, const Utilities::ivec2& _gridCoords, const bool& _bRemoveEntries)
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
	if (!m_SelectionArgs.IsOverlappingSelection(_gridCoords))
	{
		return;
	}

	Optional<Ref<Tile>> optTile = TryGetTile(_gridCoords);
	if(optTile.has_value())
	{
		if (_toCompare == Graphics::SpriteType::NONE)
			return;

		const Graphics::SpriteType tileSprite = optTile.value()->sprite;

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
		RecursivelyGenWandSelection(_toCompare, _visitedTiles, neighbour, _bRemoveEntries);
	}

	if(_bRemoveEntries)
	{
		m_SelectionArgs.RemoveTileFromWandSelection(_gridCoords);
	}
	else 
	{
		m_SelectionArgs.AddTileToWandSelection(_gridCoords);
	}
}

void WorldEditor::PickTile()
{
	using namespace App::Config;

	Optional<Ref<Tile>> optTile = TryGetTile(m_HoveredGridCell);

	if (!optTile.has_value())
		return;

	const Ref<Tile> tile = optTile.value();
	TileConfiguration.SpriteType  = tile->sprite;
	TileConfiguration.bIsWalkable = tile->bIsWalkable;
}

void WorldEditor::RenderChunkVisuals()
{
	using namespace App::Config;

	if (!SettingsConfiguration.bRenderChunkVisuals)
	{
		return;
	}

	const Utilities::ivec2 startTile  = ScreenToGridSpace(Utilities::ivec2(0, 0));
	const Utilities::ivec2 endTile    = ScreenToGridSpace(Utilities::ivec2{ Editor::s_WindowWidth, Editor::s_WindowHeight });

	const Utilities::ivec2 chunkStart = ToChunkCoords(startTile);
	const Utilities::ivec2 chunkEnd   = ToChunkCoords(endTile);

	constexpr I32 size = GRIDCELLSIZE * SIZE_CHUNK_TILES;

	for (I32 chunkX = chunkStart.x; chunkX < chunkEnd.x + 1; chunkX++)
	{
		for (I32 chunkY = chunkStart.y; chunkY < chunkEnd.y + 1; chunkY++)
		{
			const SDL_Rect rect =
			{
				chunkX * size,
				chunkY * size,
				size,
				size				
			};

			constexpr Color chunkOutlineColor = Color(70, 70, 70, 255);
			g_globals.Renderer->DrawRectOutline(rect, chunkOutlineColor, 1, 5);

			std::string coordinateString = "[";
			coordinateString.append(std::to_string(chunkX));
			coordinateString.append(" , ");
			coordinateString.append(std::to_string(chunkY));
			coordinateString.append("]");

			const Utilities::ivec2 textRenderPos = Utilities::ivec2(rect.x, rect.y) + Utilities::ivec2(20);

			TextArgs textArgs;
			textArgs.Color    = { 150, 150, 100, 150 };
			textArgs.Position = textRenderPos;
			textArgs.Text     = coordinateString;
			textArgs.TextSize = 20;
			textArgs.ZOrder   = 10;

			g_globals.Renderer->RenderText(textArgs);
		}
	}
}

void WorldEditor::PlaceBrushTiles()
{
	using namespace App::Config;

	for (I32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (I32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
		{
			const Utilities::ivec2 position = m_HoveredGridCell + Utilities::ivec2(x, y);

			if (m_SelectionArgs.bIsActive)
			{
				if(m_SelectionArgs.IsWandActive()) 
				{
					if (!m_SelectionArgs.IsOverlappingWand(position))
						continue;
				}
				else
				{
					if (!m_SelectionArgs.IsOverlappingSelection(position))
						continue;
				}
			}
			
			PlaceTile(position);
		}
	}
}

void WorldEditor::RemoveBrushTiles()
{
	using namespace App::Config;

	for (I32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
	{
		for (I32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
		{
			const Utilities::ivec2 position = m_HoveredGridCell + Utilities::ivec2{ x, y };

			RemoveTile(position);
		}
	}
}

void WorldEditor::RemoveTilesSelection()
{
	//*----------------------------------
	// If no selection is active, ignore.
	//
	if(!m_SelectionArgs.bIsActive) 
	{
		return;
	}

	for (const Utilities::ivec2& _pos : m_SelectionArgs.GetAffectedTiles())
	{
		RemoveTile(_pos);
	}

	m_SelectionArgs.SelectedTiles.clear();
}

void WorldEditor::Fill()
{
	if(!m_SelectionArgs.IsOverlapping(m_HoveredGridCell)) 
	{
		return;
	}

	for (const Utilities::ivec2& _pos : m_SelectionArgs.GetAffectedTiles())
	{
		PlaceTile(_pos);
	}
}

const bool WorldEditor::IsValidChunk(const Utilities::ivec2& _chunkCoords) const
{
	return m_Chunks.find(_chunkCoords) != m_Chunks.end();
}

void WorldEditor::RenderTiles()
{
	for(const auto&[chunkCoords, chunk] : m_Chunks) 
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
}

void WorldEditor::DrawSelection()
{
	using namespace App::Config;

	if (!m_SelectionArgs.bIsActive)
	{
		return;
	}

	//Render the dragged tiles if applicable.
	for (const DragArgs& item : m_SelectionArgs.SelectedTiles)
	{
		item.Tile->Render();
	}
	
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
		if (!m_SelectionArgs.bIsDragging)
		{
			const SDL_Rect rect
			{
				min.x  * GRIDCELLSIZE,
				min.y  * GRIDCELLSIZE,
				size.x * GRIDCELLSIZE,
				size.y * GRIDCELLSIZE
			};

			constexpr Color rectCol    = { 0, 92, 158, 30  };
			g_globals.Renderer->DrawRect(rect, rectCol, 10);

			constexpr Color outlineCol = { 0, 92, 158, 255 };
			g_globals.Renderer->DrawRectOutline(rect, outlineCol, 1, 10);
		}
	}

	//*-------------------------------
	// Draw magic wand selection field.
	//
	{
		const Color rectCol = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 50.0f, 4.0f, Time::GetElapsedTime())};

		for (const Utilities::ivec2& _tile : m_SelectionArgs.WandSelectedTiles)
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
	m_Chunks.erase(ToChunkCoords(m_HoveredGridCell));
}

const Utilities::ivec2 WorldEditor::ToChunkCoords(const Utilities::ivec2& _gridCoords) const
{
	return Utilities::ivec2
	{
		(_gridCoords.x < 0) ? ((_gridCoords.x + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.x / SIZE_CHUNK_TILES),
		(_gridCoords.y < 0) ? ((_gridCoords.y + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.y / SIZE_CHUNK_TILES)
	};
}

const Utilities::ivec2 WorldEditor::ToLocalChunkCoords(const Utilities::ivec2& _gridCoords) const
{
	return Utilities::ivec2
	{
		(_gridCoords.x % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES,
		(_gridCoords.y % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES
	};
}

const Utilities::ivec2 WorldEditor::ScreenToGridSpace(const Utilities::ivec2& _screenCoords) const
{
	Utilities::ivec2 worldCoords = Renderer::ScreenToWorld(_screenCoords);

	if (worldCoords.x < 0)
	{
		worldCoords.x -= App::Config::GRIDCELLSIZE - 1;
	}

	if (worldCoords.y < 0)
	{
		worldCoords.y -= App::Config::GRIDCELLSIZE - 1;
	}

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

			Ref<Tile> tile = m_Tiles[tileIndex];

			data["Tiles"].push_back
			(
				{
					{"X", x },
					{"Y", y },
					{"SpriteId",   static_cast<U16>(tile->sprite)},
					{"IsWalkable", tile->bIsWalkable             }
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