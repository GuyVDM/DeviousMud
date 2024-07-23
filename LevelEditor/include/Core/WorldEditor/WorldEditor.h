#pragma once
#include "Core/Core.hpp"

#include "Core/Tile/Tile.h"
#include "Core/Serialization/JsonSerializable.h"
#include "Core/WorldEditor/Selection/Selection.h"
#include "Core/Renderer/SubSprite/SubSprite.hpp"

#include "Shared/Utilities/vec2.hpp"

#include <array>
#include <map>

class Chunk;
struct Camera;

enum class e_SelectedLayer : U8;

namespace Graphics 
{
	enum class SpriteType : U16;
}

class WorldEditor 
{
public:
	void AddTileEntityTo(Ref<TileEntity> _tile, const Utilities::ivec2& _gridCoords, const e_SelectedLayer& _layer);

	void PlaceLayerEntity();

	void RemoveLayerEntity();

	void Release();

	void CloneChunk();

	void PasteChunk();

	void RemoveChunk();

	void SerializeHoveredChunk();

	void ClearSelection();

	void SetSelectionNavigatable(const bool& _bCanNavigate);

	const bool IsSelectionActive() const;

	const bool IsHoveringOverActiveChunk() const;

	const static bool IsTileVisible(const Utilities::ivec2 _gridCoords);

public:
	WorldEditor();
	virtual ~WorldEditor() = default;

private:
	const static Utilities::ivec2 ScreenToGridSpace(const Utilities::ivec2& _screenCoords);

	const Optional<Ref<TileEntity>> TryGetTileEntity(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Try remove tile from attached chunk and return shared pointer.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	Optional<Ref<TileEntity>> TryDetachTileEntity(const Utilities::ivec2& _gridCoords);
 
	Optional<Ref<Chunk>> TryGetChunk(const Utilities::ivec2& _gridCoords);

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
	std::vector<Utilities::ivec2> m_VisitedTiles;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	SelectionArgs m_SelectionArgs;

	Ref<Camera> m_Camera;

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;

	friend class EditorLayer;
};