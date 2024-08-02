#include "precomp.h"

//Official first include.
#include "Core/WorldEditor/WorldEditor.h"

#include "Core/FileHandler/FileHandler.h"
#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/Config/Time/Time.hpp"
#include "Core/Editor/Editor.h"
#include "Core/Globals/Globals.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/TileLayer/TileLayer.h"

#include "Core/Tile/Entities/TileEntity/TileEntity.h"
#include "Core/Tile/Entities/NPCEntity/NPCEntity.h"

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
	DrawHoverTileInfo();
}

void WorldEditor::LoadMap()
{
	COMDLG_FILTERSPEC _fileTypeArray[1] =
	{ L"DMEditorMapFile *.dmap*", L"*.dmap*" };

	DialogueBoxArgs args;
	args.Operation         = e_FileOperationType::FILE_LOAD;
	args.WindowTitle       = L"Load Editor Map File";
	args.FilterTypeArray   = _fileTypeArray;
	args.FilterArraySize   = 1;
	args.SelectButtonLabel = L"Load";
	args.InitDir           = "C://";

	std::string path = FileHandler::OpenFileWindow(args);

	if(path.empty()) 
	{
		return;
	}

	try
	{
		std::ifstream is(path, std::ios::binary);
		if (!is)
		{
			DEVIOUS_ERR("Failed to load map: " << path.c_str());
			return;
		}


		DEVIOUS_EVENT("Loaded map at directory: " << path);
		CleanMap();
		m_CurrentMapPath = path;
		cereal::BinaryInputArchive ar(is);
		ar(m_Chunks);

		g_globals.Renderer->UpdateMapName(m_CurrentMapPath);
	}
	catch(cereal::Exception& e) 
	{
		DEVIOUS_ERR("File is either corrupted or is not valid: " << path);
		DEVIOUS_ERR(e.what());
	}
}

void WorldEditor::SaveMap()
{
	COMDLG_FILTERSPEC _fileTypeArray[1] =
	{ L"DMEditorMapFile *.dmap*", L"*.dmap*" };

	DialogueBoxArgs args;
	args.Operation         = e_FileOperationType::FILE_SAVE;
	args.WindowTitle       = L"Save Editor Map File";
	args.FilterTypeArray   = _fileTypeArray;
	args.FilterArraySize   = 1;
	args.SelectButtonLabel = L"Save";
	args.InitialFileName   = L"Mymap.dmap";
	args.InitDir           = "C://";

	m_CurrentMapPath = FileHandler::OpenFileWindow(args);

	if (m_CurrentMapPath.empty())
	{
		return;
	}

	try
	{
		std::ofstream os(m_CurrentMapPath, std::ios::binary);
		if (!os)
		{
			DEVIOUS_ERR("Failed to save map: " << m_CurrentMapPath.c_str());
			return;
		}

		DEVIOUS_EVENT("Saved map at directory: " << m_CurrentMapPath);

		cereal::BinaryOutputArchive ar(os);
		ar(m_Chunks);

		m_CurrentMapPath = m_CurrentMapPath;
		g_globals.Renderer->UpdateMapName(m_CurrentMapPath);
	}
	catch (cereal::Exception& e)
	{
		DEVIOUS_ERR("File couldn't get saved at: " << m_CurrentMapPath);
		DEVIOUS_ERR(e.what());
	}
}

void WorldEditor::QuickSaveMap()
{
	if(m_CurrentMapPath.empty()) 
	{
		//If there's no mapfile attached to the current map,
		//We need to specify a safefile anyways before we can do any quicksaves.
		SaveMap();
		return;
	}

	try
	{
		std::ofstream os(m_CurrentMapPath, std::ios::binary);
		cereal::BinaryOutputArchive ar(os);
		ar(m_Chunks);
		DEVIOUS_EVENT("Saved map at directory: " << m_CurrentMapPath);
	}
	catch (cereal::Exception& e)
	{
		DEVIOUS_ERR("File couldn't get saved at: " << m_CurrentMapPath);
		DEVIOUS_ERR(e.what());
	}
}

void WorldEditor::ImportAndMergeMap()
{
	COMDLG_FILTERSPEC _fileTypeArray[1] =
	{ L"DMEditorMapFile *.dmap*", L"*.dmap*" };

	DialogueBoxArgs args;
	args.Operation = e_FileOperationType::FILE_LOAD;
	args.WindowTitle = L"Import and Merge";
	args.FilterTypeArray = _fileTypeArray;
	args.FilterArraySize = 1;
	args.SelectButtonLabel = L"Merge";
	args.InitialFileName = L"Mymap.dmap";
	args.InitDir = "C://";

	std::string path = FileHandler::OpenFileWindow(args);

	if (path.empty())
	{
		return;
	}

	try
	{
		std::map<Utilities::ivec2, Ref<Chunk>> tempContainer;

		std::ifstream is(path, std::ios::binary);
		if (!is)
		{
			DEVIOUS_ERR("Failed to merge map: " << path.c_str());
			return;
		}

		cereal::BinaryInputArchive ar(is);
		ar(tempContainer);

		//Copy over chunks & tiles to our scene.
		for (auto& [pos, tempChunk] : tempContainer)
		{
			if (!m_Chunks[pos])
			{
				m_Chunks[pos] = tempChunk;
				continue;
			}

			Ref<Chunk>& chunk = m_Chunks[pos];

			for (U32 i = 0; i < TILE_COUNT_CHUNK; i++)
			{
				Ref<Tile>& tempTile = tempChunk->m_Tiles[i];
				if (!tempTile)
				{
					continue;
				}

				Ref<Tile>& ourTile = chunk->m_Tiles[i];
				if (!ourTile)
				{
					ourTile = tempTile;
					continue;
				}

				for (U8 i = 0; i < static_cast<U8>(e_SelectedLayer::LAYER_COUNT); i++)
				{
					const e_SelectedLayer layer = static_cast<e_SelectedLayer>(i);

					Optional<Ref<TileEntity>> optEntt = tempTile->TryGetEntity(layer);
					if (optEntt.has_value())
					{
						ourTile->InsertLayerEntity(optEntt.value(), layer);
					}
				}
			}
		}

		DEVIOUS_EVENT("Succesfully merged the scene with: " << path);
	}
	catch (cereal::Exception& e)
	{
		DEVIOUS_ERR("Failed to merge scene with file: " << m_CurrentMapPath);
		DEVIOUS_ERR(e.what());
	}
}

void WorldEditor::CleanMap()
{
	m_CurrentMapPath = "";
	ClearSelection(); 
	m_Chunks.clear();
	g_globals.Camera->Reset();
	g_globals.Renderer->UpdateMapName(m_CurrentMapPath);
}

void WorldEditor::AddTileEntityTo(Ref<TileEntity> _tile, const Utilities::ivec2& _gridCoords, const e_SelectedLayer& _layer)
{
	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(_gridCoords);

	if (!IsValidChunk(chunkCoords))
	{
		m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
	}

	Ref<Chunk>& chunk = m_Chunks[chunkCoords];

	//Move over the TileEntity to it's new parent tile.
	chunk->AddToTile
	(
		Chunk::ToLocalChunkCoords(_gridCoords),
		_tile,
		_layer
	);
}

void WorldEditor::PlaceLayerEntity()
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

void WorldEditor::RemoveLayerEntity() 
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
				RemoveFill();
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
	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(m_HoveredGridCell);

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

	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(m_HoveredGridCell);

	Ref<Chunk> cbChunk = TileConfiguration.ChunkClipboard.lock();

	//If the chunk were trying to paste in is the same as the one in the clipboard,
	//We want to skip.
	if (m_Chunks[chunkCoords] == cbChunk)
	{
		return;
	}

	m_Chunks.erase(chunkCoords);
	Ref<Chunk> chunk = m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);

	//*-------------------
	// Duplicate the chunk.
	//
	for (U32 x = 0; x < SIZE_CHUNK_TILES; x++)
	{
		for (U32 y = 0; y < SIZE_CHUNK_TILES; y++)
		{
			const U32 i = Chunk::ToTileIndex(Utilities::ivec2(x, y));

			if (cbChunk->m_Tiles[i] == nullptr)
			{
				continue;
			}

			Ref<Tile> tileCopy = std::make_shared<Tile>(cbChunk->m_Tiles[i].get());
			chunk->AddTile(tileCopy);
		}
	}
}

const Optional<Ref<TileEntity>> WorldEditor::TryGetTileEntity(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords      = Chunk::ToChunkCoords(_gridCoords);
	const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(_gridCoords);
	
	if (!IsValidChunk(chunkCoords)) 
	{
		return std::nullopt;
	}

	const e_SelectedLayer layer = App::Config::TileConfiguration.CurrentLayer;
	return m_Chunks[chunkCoords]->TryGetTileEntity(localChunkCoords, layer);
}

Optional<Ref<TileEntity>> WorldEditor::TryDetachTileEntity(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(_gridCoords);

	if (!IsValidChunk(chunkCoords))
	{
		return std::nullopt;
	}

	const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(_gridCoords);
	const e_SelectedLayer  layer            = App::Config::TileConfiguration.CurrentLayer;

	return m_Chunks[chunkCoords]->TryDetachFromTile(localChunkCoords, layer);
}

Optional<Ref<Chunk>> WorldEditor::TryGetChunk(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(_gridCoords);

	if(IsValidChunk(chunkCoords)) 
	{
		return m_Chunks[chunkCoords];
	}

	return std::nullopt;
}

void WorldEditor::DrawHoverTileInfo()
{
	using namespace App::Config;

	Optional<Ref<TileEntity>> tileEntt = TryGetTileEntity(m_HoveredGridCell);

	if(!tileEntt.has_value()) 
	{
		return;
	}

	switch(TileConfiguration.CurrentLayer) 
	{
		case e_SelectedLayer::LAYER_NPC:
		{
			if (!SettingsConfiguration.bShowNpcParams)
			{
				return;
			}

			Ref<NPCEntity> npc = std::static_pointer_cast<NPCEntity>(tileEntt.value());

			const Utilities::ivec2 textPos = Utilities::ivec2(m_HoveredGridCell.x * GRIDCELLSIZE + (I32)(roundf((float)(GRIDCELLSIZE * 0.5f))), m_HoveredGridCell.y * GRIDCELLSIZE);

			TextArgs args;
			// Render Npc id.
			{
				args.Position = textPos - Utilities::ivec2(0, 10);
				args.Text = "Npcid: " + std::to_string(npc->NpcId);
				args.TextSize = 56;
				args.Color = { 255,255,255,255 };
				args.ZOrder = 100;
				g_globals.Renderer->RenderText(args, 0.125f, e_TextAlignment::MIDDLE);
			}

			// Render respawn time
			{
				args.Position = textPos - Utilities::ivec2(0, 20);
				args.Text = "Name: " + get_npc_definition(npc->NpcId).name;
				g_globals.Renderer->RenderText(args, 0.125f, e_TextAlignment::MIDDLE);
			}

			// Render respawn time
			{
				args.Text = "Respawntime: " + std::to_string(npc->RespawnTime);
				args.Position = textPos - Utilities::ivec2(0, 30);
				g_globals.Renderer->RenderText(args, 0.125f, e_TextAlignment::MIDDLE);
			}
		}
		break;
	}
}

void WorldEditor::HandleShortCuts()
{
	const bool bCtrlPressed = (g_Input->GetKey(SDLK_RCTRL) || g_Input->GetKey(SDLK_LCTRL));
	const bool bAltPressed = (g_Input->GetKey(SDLK_RALT) || g_Input->GetKey(SDLK_LALT));

	//*-----------------------------------
	// Delete all tiles that are selected.
	//
	{
		const bool bDeleteKeyPressed = g_Input->GetKeyDown(SDLK_DELETE);

		if (bDeleteKeyPressed)
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

	//*------------------------
	// Quick editor shortcuts that're bound to ctrl.
	//
	{
		if (bCtrlPressed)
		{
			if (g_Input->GetKeyDown(SDLK_s))
			{
				QuickSaveMap();
			}

			if(g_Input->GetKeyDown(SDLK_r)) 
			{
				g_globals.Camera->Reset();
			}
			return;
		}
	}

	//*-------------------------
	// Shortcut to editor tools
	//
	{
		if (g_Input->GetKeyDown(SDLK_f))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_FILL;
		}

		if (g_Input->GetKeyDown(SDLK_p))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_PICKER;
		}

		if (g_Input->GetKeyDown(SDLK_s))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_SELECTION;
		}

		if (g_Input->GetKeyDown(SDLK_d))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_DRAG;
		}

		if (g_Input->GetKeyDown(SDLK_b))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_BRUSH;
		}

		if (g_Input->GetKeyDown(SDLK_w))
		{
			App::Config::TileConfiguration.InteractionMode = e_InteractionMode::MODE_WAND;
		}

		if (g_Input->GetKeyDown(SDLK_r))
		{
			g_globals.Camera->Reset();
		}
	}
}

void WorldEditor::PlaceTileEntity(const Utilities::ivec2& _gridCoords)
{
	using namespace App::Config;

	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(_gridCoords);
	if (!IsValidChunk(chunkCoords)) 
	{
		m_Chunks[chunkCoords] = std::make_shared<Chunk>(chunkCoords);
	}

	const e_SelectedLayer layer = TileConfiguration.CurrentLayer;


	//*--------------------------------------------------------------------
	// Create the tile entity based on the layer we have currently selected,
	// and place it on top of the hovered tile afterwards.
	Ref<TileEntity> tileEntity;

	switch(layer)
	{
		case e_SelectedLayer::LAYER_NPC:
		{
			Ref<NPCEntity> npcEntity = std::make_shared<NPCEntity>();
			npcEntity->SpriteType = TileConfiguration.SelectedNPC.sprite;
			npcEntity->Frame = 0;
			npcEntity->NpcId = TileConfiguration.NPCid;
			npcEntity->RespawnTime = TileConfiguration.NPCRespawnTime;
			tileEntity = npcEntity;
		}
		break;

		default:
		{
			tileEntity = std::make_shared<TileEntity>();
			tileEntity->SpriteType = TileConfiguration.Sprite.SpriteType;
			tileEntity->Frame      = TileConfiguration.Sprite.Frame;
		}
		break;
	}

	const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(_gridCoords);
	m_Chunks[chunkCoords]->AddToTile(localChunkCoords, tileEntity, layer);
}

void WorldEditor::RemoveFromTile(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(_gridCoords);

	Optional<Ref<Chunk>> optChunk = TryGetChunk(_gridCoords);

	if(optChunk.has_value()) 
	{
		Ref<Chunk> chunk = optChunk.value();

		const e_SelectedLayer layer = App::Config::TileConfiguration.CurrentLayer;
		chunk->RemoveFromTile(Chunk::ToLocalChunkCoords(_gridCoords), layer);
		
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
			Optional<Ref<TileEntity>> optTileEntity = TryDetachTileEntity(selectionPos);

			if (!optTileEntity.has_value())
			{
				continue;
			}

			DragArgs args;
			args.TileEntity = optTileEntity.value();
			args.StartGridPos = selectionPos;
			args.FromLayer = App::Config::TileConfiguration.CurrentLayer;

			m_SelectionArgs.SelectedDraggingTiles.push_back(args);
		}
	}

	m_SelectionArgs.MoveSelectionRelativeTo(m_HoveredGridCell);
}

void WorldEditor::TryPlaceSelectedTiles()
{
	for(const DragArgs& _args : m_SelectionArgs.SelectedDraggingTiles) 
	{
		const Utilities::ivec2 gridPos = _args.CurrentGridPos;
		AddTileEntityTo(_args.TileEntity, gridPos, _args.FromLayer);
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
				m_SelectionArgs.RemoveTileCoordFromSelection(_pos);
			}
			break;

			case e_SelectionMode::DEFAULT:
			case e_SelectionMode::ADDITION:
			{
				m_SelectionArgs.AddTileCoordToSelection(_pos);
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
	return IsValidChunk(Chunk::ToChunkCoords(m_HoveredGridCell));
}

const bool WorldEditor::IsTileVisible(const Utilities::ivec2 _gridCoords)
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

	SubSprite subsprite = SubSprite(Graphics::SpriteType::NONE, 0);

	if(IsHoveringOverActiveChunk())
	{
		Optional<Ref<TileEntity>> optTileEntt = TryGetTileEntity(m_HoveredGridCell);

		if (optTileEntt.has_value())
		{
			Ref<TileEntity> tileEntity = optTileEntt.value();
			subsprite = SubSprite
			(
				tileEntity->SpriteType,
				tileEntity->Frame
			);
		}
	}

	//Recursively draw the wand selection.
	{
		RecursivelyGenWandSelection(subsprite, m_HoveredGridCell, bAltPressed);
		m_VisitedTiles.clear();
	}
}

void WorldEditor::RecursivelyGenWandSelection(const SubSprite& _sprite, const Utilities::ivec2& _gridCoords, const bool& _bRemoveEntries)
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

	if (!IsTileVisible(_gridCoords))
	{
		return;
	}
	 
	Optional<Ref<TileEntity>> optTileEntt = TryGetTileEntity(_gridCoords);
	{
		if (!optTileEntt.has_value())
		{
			if (_sprite.SpriteType != Graphics::SpriteType::NONE)
			{
				return;
			}
		}
		else
		{
			Ref<TileEntity> tileEntity = optTileEntt.value();

			//Check if the compared two sprites have matching visuals, if not, we stop.
			const SubSprite toCompare = SubSprite(tileEntity->SpriteType, tileEntity->Frame);
			if (toCompare != _sprite)
			{
				return;
			}
		}
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
		RecursivelyGenWandSelection(_sprite, neighbour, _bRemoveEntries);
	}

	if(_bRemoveEntries)
	{
		m_SelectionArgs.RemoveTileCoordFromSelection(_gridCoords);
	}
	else 
	{
		m_SelectionArgs.AddTileCoordToSelection(_gridCoords);
	}
}

void WorldEditor::PickTile()
{
	using namespace App::Config;

	//*--------------------------
	// Try and get tile entity from tile.
	//
	Ref<TileEntity> tileEntity;
	{
		Optional<Ref<TileEntity>> optTileEntt = TryGetTileEntity(m_HoveredGridCell);

		if (!optTileEntt.has_value())
		{
			return;
		}

		//Extract the tile entity.
		tileEntity = optTileEntt.value();
	}

	switch (TileConfiguration.CurrentLayer)
	{
		case e_SelectedLayer::LAYER_NPC:
		{
			Ref<NPCEntity> npc = std::static_pointer_cast<NPCEntity>(tileEntity);

			TileConfiguration.NPCid          = npc->NpcId;
			TileConfiguration.NPCRespawnTime = npc->RespawnTime;
			TileConfiguration.SelectedNPC    = get_npc_definition(npc->NpcId);
		}
		break;
	}

	TileConfiguration.Sprite = SubSprite
	(
		tileEntity->SpriteType,
		tileEntity->Frame
	);
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

	const Utilities::ivec2 chunkStart = Chunk::ToChunkCoords(startTile);
	const Utilities::ivec2 chunkEnd   = Chunk::ToChunkCoords(endTile);

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
				textArgs.Color = { 255, 255, 255, 150 };
				textArgs.Position = textRenderPos;
				textArgs.Text = coordinateStream.str();
				textArgs.TextSize = 40;
				textArgs.ZOrder = 10;

				g_globals.Renderer->RenderText(textArgs, 0.5f);
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

			PlaceTileEntity(position);
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

			RemoveFromTile(position);
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
		RemoveFromTile(_pos);
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
		PlaceTileEntity(_pos);
	}
}

void WorldEditor::RemoveFill()
{
	for(Utilities::ivec2& pos : m_SelectionArgs.SelectedTiles) 
	{
		if(!m_SelectionArgs.IsOverlapping(pos)) 
		{
			continue;
		}

		RemoveFromTile(pos);
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
		if(!chunk->IsVisible()) 
		{
			continue;
		}

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

	SDL_Rect rect{};

	switch(TileConfiguration.InteractionMode) 
	{
	case e_InteractionMode::MODE_BRUSH:
	{
		for (I32 x = 0; x < SettingsConfiguration.BrushSize.x; x++)
		{
			for (I32 y = 0; y < SettingsConfiguration.BrushSize.y; y++)
			{
				const Utilities::ivec2 pos =
				{
					(m_HoveredGridCell.x + x) * GRIDCELLSIZE,
				    (m_HoveredGridCell.y + y) * GRIDCELLSIZE
				};

				constexpr Color previewHighlight = { 150, 150, 0, 140 };

				RenderQuery query;
				query.Color      = previewHighlight;
				query.Frame      = TileConfiguration.Sprite.Frame;
				query.Position   = pos;
				query.Size       = GRIDCELLSIZE;
				query.SpriteType = TileConfiguration.Sprite.SpriteType;

				g_globals.Renderer->Render(query, 10);
			}
		}
	}
		return;

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

	const U8 occilateAlpha = DMath::Occilate<U8>(30.0f, 100.0f, 1.5f, Time::GetElapsedTime());
	const Color rectCol = { 255, 255, 0, occilateAlpha };
	const Color outlineCol = { 120, 120, 0, occilateAlpha };

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
		RenderQuery query;
		query.Color      = { 255, 255, 255, 125 };
		query.SpriteType = item.TileEntity->SpriteType;
		query.Frame      = item.TileEntity->Frame;
		query.Position   = item.CurrentGridPos * GRIDCELLSIZE;
		query.Size       = GRIDCELLSIZE;

		g_globals.Renderer->Render(query, 100);
	}

	//*-------------------------------
	// Draw all selected tiles.
	//
	{
		const Color rectCol = { 0, 92, 158, DMath::Occilate<U8>(140.0f, 180.0f, 5.0f, Time::GetElapsedTime())};

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

void WorldEditor::SetSelectionNavigatable(const bool& _bCanNavigate)
{
	for(const Utilities::ivec2& gridCoord : m_SelectionArgs.SelectedTiles) 
	{
		const Utilities::ivec2 chunkCoords = Chunk::ToChunkCoords(gridCoord);

		if(!IsValidChunk(chunkCoords)) 
		{
			continue;
		}

		const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(gridCoord);

		Ref<Chunk>& chunk = m_Chunks[chunkCoords];

		const U32 tileIndex = Chunk::ToTileIndex(localChunkCoords);
		if(!chunk->IsValidIndex(tileIndex)) 
		{
			continue;
		}

		chunk->m_Tiles[tileIndex]->bIsWalkable = _bCanNavigate;
	}
}

void WorldEditor::RemoveChunk()
{
	m_Chunks.erase(Chunk::ToChunkCoords(m_HoveredGridCell));
}

const Utilities::ivec2 WorldEditor::ScreenToGridSpace(const Utilities::ivec2& _screenCoords)
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

