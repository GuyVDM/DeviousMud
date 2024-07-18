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
	HandleShortCuts();
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
	// Move the tile over to its new chunk
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
				HandleWandSelection();
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
			CollapseSelection();
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

void WorldEditor::HandleShortCuts()
{
	const bool bCtrlPressed = (g_Input->GetKey(SDLK_RCTRL) || g_Input->GetKey(SDLK_LCTRL));
	const bool bAltPressed = (g_Input->GetKey(SDLK_RALT) || g_Input->GetKey(SDLK_LALT));

	//*-----------------------------------
	// Delete all tiles that are selected.
	//
	{
		const bool bDelete = g_Input->GetKeyDown(SDLK_DELETE);

		if (bDelete)
		{
			RemoveTilesSelection();
		}
	}

	//*-------------------
	// Set selection mode.
	//
	{
		e_SelectionMode mode = e_SelectionMode::DEFAULT;

		if (bCtrlPressed)
		{
			mode = e_SelectionMode::ADDITION;
		}
		else
		if (bAltPressed)
		{
			mode = e_SelectionMode::SUBTRACTION;
		}

		m_SelectionArgs.SelectionMode = mode;
	}

	//*-------------------------
	// Shortcut to editor tools
	//
	{
		if(bCtrlPressed) 
		{
			if(g_Input->GetKey(SDLK_f)) 
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_FILL;
			}

			if (g_Input->GetKey(SDLK_p))
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_PICKER;
			}

			if (g_Input->GetKey(SDLK_s))
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_SELECTION;
			}

			if (g_Input->GetKey(SDLK_d))
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_DRAG;
			}

			if (g_Input->GetKey(SDLK_b))
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_BRUSH;
			}

			if (g_Input->GetKey(SDLK_w))
			{
				App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_WAND;
			}

			if (g_Input->GetKey(SDLK_r))
			{
				g_globals.Camera->Reset();
			}
		}
	}
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
	if (m_SelectionArgs.IsEmpty())
	{
		return;
	}

	if (!m_SelectionArgs.bIsDragging)
	{
		m_SelectionArgs.bIsDragging        = true;
		m_SelectionArgs.StartDragPos       = m_HoveredGridCell;
		m_SelectionArgs.SelectedTilesStart = m_SelectionArgs.SelectedTiles;
		
		//Collect tiles from wand
		for (const Utilities::ivec2& selectionPos : m_SelectionArgs.GetSelectedGridSpaces())
		{
			Optional<Ref<Tile>> optTile = TryDetachTile(selectionPos);

			if (!optTile.has_value())
			{
				continue;
			}

			DragArgs args;
			args.Tile  = optTile.value();
			args.StartGridPos = (args.Tile->ChunkCoords * SIZE_CHUNK_TILES) + args.Tile->LocalChunkCoords;
			m_SelectionArgs.SelectedDraggingTiles.push_back(args);
		}
	}

	m_SelectionArgs.MoveSelectionRelativeTo(m_HoveredGridCell);
}

void WorldEditor::TryPlaceSelectedTiles()
{
	for(const DragArgs& _args : m_SelectionArgs.SelectedDraggingTiles) 
	{
		const Utilities::ivec2 gridPos = (_args.Tile->ChunkCoords * SIZE_CHUNK_TILES) + _args.Tile->LocalChunkCoords;
		MoveTileTo(_args.Tile, gridPos);
	}

	m_SelectionArgs.SelectedDraggingTiles.clear();
	m_SelectionArgs.bIsDragging = false;
}

void WorldEditor::CreateSelection()
{
	SelectionField& field = m_SelectionArgs.Field;

	if (m_SelectionArgs.SelectionMode == e_SelectionMode::DEFAULT)
	{
		m_SelectionArgs.SelectedTiles.clear();
	}

	//Enable Grid Selection.
	if(!field.Begin(m_HoveredGridCell)) 
	{
		field.DrawRectTo(m_HoveredGridCell);
	}
}

void WorldEditor::CollapseSelection()
{
	for(const Utilities::ivec2& _pos : m_SelectionArgs.Field.GetTilesAndCollapse()) 
	{
		switch(m_SelectionArgs.SelectionMode) 
		{
			case e_SelectionMode::SUBTRACTION:
			{
				m_SelectionArgs.RemoveTileFromSelection(_pos);
			}
			break;

			case e_SelectionMode::DEFAULT:
			case e_SelectionMode::ADDITION:
			{
				m_SelectionArgs.AddTileToSelection(_pos);
			}
			break;
		}
	}
}

void WorldEditor::ClearSelection()
{
	TryPlaceSelectedTiles();

	m_SelectionArgs.SelectedDraggingTiles.clear();
	m_SelectionArgs.SelectedTiles.clear();
	m_SelectionArgs.StartDragPos = { 0 };
	m_SelectionArgs.bIsDragging  = false;
}

const bool WorldEditor::IsSelectionActive() const
{
	return !m_SelectionArgs.IsEmpty();
}

const bool WorldEditor::IsHoveringOverActiveChunk() const
{
	return IsValidChunk(ToChunkCoords(m_HoveredGridCell));
}

const bool WorldEditor::IsTileVisible(const Utilities::ivec2 _gridCoords) const
{
	const Utilities::ivec2 pointA = ScreenToGridSpace(Utilities::ivec2(0, 0));
	const Utilities::ivec2 pointB = ScreenToGridSpace(Utilities::ivec2(Editor::s_WindowWidth, Editor::s_WindowHeight));

	const Utilities::ivec2 min = { std::min<I32>(pointA.x, pointB.x), std::min<I32>(pointA.y, pointB.y) };
	const Utilities::ivec2 max = { std::max<I32>(pointA.x, pointB.x), std::max<I32>(pointA.y, pointB.y) };

	return (_gridCoords.x >= min.x && _gridCoords.x <= max.x && _gridCoords.y >= min.y && _gridCoords.y <= max.y);
}

void WorldEditor::HandleWandSelection()
{
	const bool bAltPressed  = g_Input->GetKey(SDLK_RALT)  || g_Input->GetKey(SDLK_LALT);

	const bool bNoReset = m_SelectionArgs.SelectionMode != e_SelectionMode::DEFAULT;
	if (!bNoReset)
	{
		//Clear the previous selection.
		m_SelectionArgs.SelectedTiles.clear();
	}

	Graphics::SpriteType type = Graphics::SpriteType::NONE;

	if(IsHoveringOverActiveChunk())
	{
		Optional<Ref<Tile>> tile = TryGetTile(m_HoveredGridCell);

		if (tile.has_value())
		{
			type = tile.value()->sprite;
		}
	}

	//Recursively draw the wand selection.
	{
		RecursivelyGenWandSelection(type, m_HoveredGridCell, bAltPressed);
		m_VisitedTiles.clear();
	}
}

void WorldEditor::RecursivelyGenWandSelection(const Graphics::SpriteType _toCompare, const Utilities::ivec2& _gridCoords, const bool& _bRemoveEntries)
{
	//*-------------------------------------------
	// If we already visited this tile, ignore it.
	//
	for(U32 i = 0; i < m_VisitedTiles.size(); i++) 
	{
		if(m_VisitedTiles[i] == _gridCoords) 
		{
			return;
		}
	}

	//Mark this as visited.
	m_VisitedTiles.push_back(_gridCoords);

	if(_toCompare == Graphics::SpriteType::NONE) 
	{
		if(!IsTileVisible(_gridCoords)) 
		{
			return;
		}
	}

	Optional<Ref<Tile>> tile = TryGetTile(_gridCoords);

	if(!tile.has_value())
	{
		if(_toCompare != Graphics::SpriteType::NONE) 
		{
			return;
		}
	}
	else
	if (tile.value()->sprite != _toCompare)
	{
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
		RecursivelyGenWandSelection(_toCompare, neighbour, _bRemoveEntries);
	}

	if(_bRemoveEntries)
	{
		m_SelectionArgs.RemoveTileFromSelection(_gridCoords);
	}
	else 
	{
		m_SelectionArgs.AddTileToSelection(_gridCoords);
	}
}

void WorldEditor::PickTile()
{
	using namespace App::Config;

	Optional<Ref<Tile>> optTile = TryGetTile(m_HoveredGridCell);

	if (!optTile.has_value()) 
	{
		return;
	}

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

			//*----------------------
			// Render chunk outlines.
			//
			{
				constexpr Color chunkOutlineColor = Color(70, 70, 70, 255);
				g_globals.Renderer->DrawRectOutline(rect, chunkOutlineColor, 1, 5);
			}

			//*------------------------------------------------------
			// Render chunk coordinates in the top left of each chunk
			// in text format.
			//
			{
				std::ostringstream coordinateStream;
				coordinateStream << "[" << chunkX << " , " << chunkY << "]";

				const Utilities::ivec2 offset(20, 20);
				const Utilities::ivec2 textRenderPos = Utilities::ivec2(rect.x, rect.y) + offset;

				TextArgs textArgs;
				textArgs.Color = { 150, 150, 100, 150 };
				textArgs.Position = textRenderPos;
				textArgs.Text = coordinateStream.str();
				textArgs.TextSize = 20;
				textArgs.ZOrder = 10;

				g_globals.Renderer->RenderText(textArgs);
			}
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

			if (!m_SelectionArgs.IsEmpty())
			{
				if (!m_SelectionArgs.IsOverlapping(position))
				{
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
	if(m_SelectionArgs.IsEmpty()) 
	{
		return;
	}

	for (const Utilities::ivec2& _pos : m_SelectionArgs.GetSelectedGridSpaces())
	{
		RemoveTile(_pos);
	}

	ClearSelection();
}

void WorldEditor::Fill()
{
	if(!m_SelectionArgs.IsOverlapping(m_HoveredGridCell)) 
	{
		return;
	}

	for (const Utilities::ivec2& _pos : m_SelectionArgs.GetSelectedGridSpaces())
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

	//*----------------------
	// Draw selection field.
	//
	SelectionField field = m_SelectionArgs.Field;
	if (field.IsActive())
	{
		Utilities::ivec2 from, to;
		field.GetFromTo(from, to);

		const Utilities::ivec2 min =
		{
			std::min(from.x, to.x),
			std::min(from.y, to.y)
		};

		const Utilities::ivec2 max =
		{
			std::max(from.x, to.x),
			std::max(from.y, to.y)
		};

		const Utilities::ivec2 size =
		{
			static_cast<I32>(abs(min.x - max.x)) + 1,
			static_cast<I32>(abs(min.y - max.y)) + 1
		};

		const SDL_Rect rect
		{
			min.x * GRIDCELLSIZE,
			min.y * GRIDCELLSIZE,
			size.x * GRIDCELLSIZE,
			size.y * GRIDCELLSIZE
		};

		constexpr Color rectCol = { 0, 92, 158, 30 };
		g_globals.Renderer->DrawRect(rect, rectCol, 10);

		constexpr Color outlineCol = { 0, 92, 158, 255 };
		g_globals.Renderer->DrawRectOutline(rect, outlineCol, 1, 10);
	}

	//*------------------------------
	// Render dragged selected tiles.
	//
	for (const DragArgs& item : m_SelectionArgs.SelectedDraggingTiles)
	{
		item.Tile->Render();
	}

	//*-------------------------------
	// Draw magic wand selection field.
	//
	{
		const Color rectCol = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 50.0f, 4.0f, Time::GetElapsedTime())};

		for (const Utilities::ivec2& _tile : m_SelectionArgs.SelectedTiles)
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