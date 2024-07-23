#pragma once
#include "Core/Core.hpp"

#include "Core/Tile/Tile.h"
#include "Core/WorldEditor/TileLayer/TileLayer.h"

#include "Shared/Utilities/vec2.hpp"

class SelectionField 
{
public:
	/// <summary>
	/// Whether this selection field is currently active.
	/// </summary>
	/// <returns></returns>
	const bool IsActive() const;

	/// <summary>
	/// Get the selection field points.
	/// </summary>
	/// <param name="_from"></param>
	/// <param name="_to"></param>
	void GetFromTo(Utilities::ivec2& _from, Utilities::ivec2& _to) const;

	/// <summary>
	/// Start tracking the selection field.
	/// </summary>
	/// <param name="_a"></param>
	/// <param name="_b"></param>
	bool Begin(const Utilities::ivec2& _a);

	/// <summary>
	/// Draws a selection field from the position set at Begin()
	/// to the position specified within _b.
	/// </summary>
	/// <param name="_b"></param>
	void DrawRectTo(const Utilities::ivec2& _b);

	/// <summary>
	/// Collapse the selection field and get every tile that was selected within.
	/// </summary>
	/// <returns></returns>
	std::vector<Utilities::ivec2> GetTilesAndCollapse();

private:
	bool                          bIsActive  = false;
	Utilities::ivec2              PointA = { 0 };
	Utilities::ivec2              PointB = { 0 };
};

struct DragArgs
{
	Ref<TileEntity>  TileEntity;
	Utilities::ivec2 StartGridPos;
	Utilities::ivec2 CurrentGridPos;
	e_SelectedLayer  FromLayer;
};

enum class e_SelectionMode
{
	DEFAULT = 0x00,
	SUBTRACTION,
	ADDITION
};

struct SelectionArgs 
{
	e_SelectionMode               SelectionMode = e_SelectionMode::DEFAULT;

	SelectionField                Field;
	std::vector<Utilities::ivec2> SelectedTiles;

	bool                          bIsDragging    = false;
	std::vector<Utilities::ivec2> SelectedTilesStart;
	Utilities::ivec2              StartDragPos   = Utilities::ivec2(0, 0);
	std::vector<DragArgs>         SelectedDraggingTiles;

	/// <summary>
	/// Returns true is there's no tiles selected.
	/// </summary>
	/// <returns></returns>
	const bool IsEmpty() const;

	/// <summary>
	/// Adds the tile to wand selection field.
	/// </summary>
	/// <param name="_gridPos"></param>
	void AddTileCoordToSelection(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Removes the tile from the wand selection field.
	/// </summary>
	/// <param name="_gridPos"></param>
	void RemoveTileCoordFromSelection(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Moves the entirety of the selection with the start dragging position relative
	/// to the gridcoords.
	/// </summary>
	/// <param name="_gridCoords"></param>
	void MoveSelectionRelativeTo(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Returns a vector of all currently selected gridspaces.
	/// </summary>
	/// <returns></returns>
	const std::vector<Utilities::ivec2> GetSelectedGridSpaces() const;

	/// <summary>
	/// Returns whether the coordinate is overlapping with any 
	/// of the selected gridspaces.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlapping(const Utilities::ivec2& _gridCoords) const;
};