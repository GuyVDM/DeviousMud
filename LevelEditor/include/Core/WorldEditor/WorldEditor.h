#pragma once
#include "Core/Core.hpp"

#include "Core/Tile/Tile.h"
#include "Core/Serialization/JsonSerializable.h"
#include "Core/WorldEditor/Selection/Selection.h"
#include "Core/Renderer/SubSprite/SubSprite.hpp"

#include "Shared/Utilities/vec2.hpp"

#include <array>
#include <map>

#include "Core/WorldEditor/Chunk/Chunk.h"

struct Camera;

enum class e_SelectedLayer : U8;

namespace Graphics 
{
	enum class SpriteType : U16;
}

class WorldEditor
{
public:
	/// <summary>
	/// Will wipe the entire editor clean and removes path bound to map file.
	/// </summary>
	void CleanMap();

	/// <summary>
	/// Will try to override the current loaded file if it exists, if not, will
	/// just do the regular SaveMap() and ask for file name and directory to save your changes to.
	/// </summary>
	void QuickSaveMap();

	/// <summary>
	/// Asks for file name and directory to save your changes to.
	/// </summary>
	void SaveMap();

	/// <summary>
	/// Load map from directory.
	/// </summary>
	void LoadMap();

	/// <summary>
	/// Try and place a TileEntity on coordinates bound to m_HoveredGridCell & based on the current
	/// selected layer.
	/// </summary>
	void PlaceLayerEntity();

	/// <summary>
	/// Try and remove a TileEntity on coordinates bound to m_HoveredGridCell & based on the current
	/// selected layer.
	/// </summary>
	void RemoveLayerEntity();

	/// <summary>
	/// Gets called when the right mouse button is released.
	/// </summary>
	void Release();

	/// <summary>
	/// Save a shared ptr reference to the chunk you want to clone.
	/// </summary>
	void CloneChunk();

	/// <summary>
	/// Will duplicate the current state of the cloned chunk over to the chunk you are hovering.
	/// </summary>
	void PasteChunk();

	/// <summary>
	/// Removes the hovered chunk and all tiles associated with that chunk specifically.
	/// </summary>
	void RemoveChunk();

	/// <summary>
	/// Clears any existing selection field.
	/// </summary>
	void ClearSelection();

	/// <summary>
	/// Turns all selected tiles into a walkable area.
	/// </summary>
	/// <param name="_bCanNavigate"></param>
	void SetSelectionNavigatable(const bool& _bCanNavigate);

	/// <summary>
	/// Add a tile entity directly to the specified grid coordinates.
	/// </summary>
	/// <param name="_tile"></param>
	/// <param name="_gridCoords"></param>
	/// <param name="_layer"></param>
	void AddTileEntityTo(Ref<TileEntity> _tile, const Utilities::ivec2& _gridCoords, const e_SelectedLayer& _layer);

	/// <summary>
	/// Returns true if a selection field is active.
	/// </summary>
	/// <returns></returns>
	const bool IsSelectionActive() const;

	/// <summary>
	/// Returns true if you're hovering over a registered chunk,
	/// registed means that the chunk contains any registered tiles.
	/// </summary>
	/// <returns></returns>
	const bool IsHoveringOverActiveChunk() const;

	/// <summary>
	/// Returns true if the tile is partially or entirely visible within screenspace.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const static bool IsTileVisible(const Utilities::ivec2 _gridCoords);

public:
	WorldEditor();
	virtual ~WorldEditor() = default;

private:
	const static Utilities::ivec2 ScreenToGridSpace(const Utilities::ivec2& _screenCoords);

	const Optional<Ref<TileEntity>> TryGetTileEntity(const Utilities::ivec2& _gridCoords);

	Optional<Ref<TileEntity>> TryDetachTileEntity(const Utilities::ivec2& _gridCoords);
 
	Optional<Ref<Chunk>> TryGetChunk(const Utilities::ivec2& _gridCoords);

	void DrawHoverTileInfo();

	void HandleShortCuts();

	void Update();

	void PlaceTileEntity(const Utilities::ivec2& _gridCoords);

	void RemoveFromTile(const Utilities::ivec2& _gridCoords);

	void DragSelectedTiles();

	void TryPlaceSelectedTiles();

	void CreateSelection();

	void CollapseSelection();

	void HandleWandSelection();

	void RecursivelyGenWandSelection(const SubSprite& _sprite, const Utilities::ivec2& _gridCoords, const bool& _bRemoveEntries = false);

	void PickTile();

	void RenderChunkVisuals();

	void PlaceBrushTiles();

	void RemoveBrushTiles();

	void RemoveTilesSelection();

	void Fill();

	void RemoveFill();

	void RenderTiles();

	void DrawBrush();

	void DrawSelection();

	const Utilities::ivec2 GetHoveredGridCell() const;

	const bool IsValidChunk(const Utilities::ivec2& _chunkCoords) const;

private:
	std::string m_CurrentMapPath = "";

	std::vector<Utilities::ivec2> m_VisitedTiles;
	
	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	SelectionArgs m_SelectionArgs;

	Ref<Camera> m_Camera;

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;

	friend class EditorLayer;
};