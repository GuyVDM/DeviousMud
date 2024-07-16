#pragma once
#include "Core/Core.hpp"

#include "Core/Tile/Tile.h"

#include "Shared/Utilities/vec2.hpp"

struct DragArgs 
{
	Ref<Tile>        Tile;
	Utilities::ivec2 StartGridPos;
};

struct SelectionArgs 
{
	bool                          bIsActive    = false;
	bool                          bIsInteracting = false;
	Utilities::ivec2              PointA  = { 0 };
	Utilities::ivec2              PointB  = { 0 };
	std::vector<Utilities::ivec2> WandSelectedTiles;

	bool                          bIsDragging    = false;
	Utilities::ivec2              startPointA    = { 0 };
	Utilities::ivec2              startPointB    = { 0 };
	Utilities::ivec2              startDragPos   = { 0 };
	std::vector<DragArgs>         SelectedTiles;
	std::vector<Utilities::ivec2> StartWandSelectedTiles;

	/// <summary>
	/// Adds the tile to wand selection field.
	/// </summary>
	/// <param name="_gridPos"></param>
	void AddTileToWandSelection(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Removes the tile from the wand selection field.
	/// </summary>
	/// <param name="_gridPos"></param>
	void RemoveTileFromWandSelection(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Moves the entirety of the selection with the start dragging position relative
	/// to the gridcoords.
	/// </summary>
	/// <param name="_gridCoords"></param>
	void MoveSelectionRelativeTo(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Returns all selection or wand affected grid coordinates.
	/// </summary>
	/// <returns></returns>
	const std::vector<Utilities::ivec2> GetAffectedTiles() const;

	/// <summary>
	/// Returns whether the coordinate is overlapping with the selection.
	/// Or if the wand is enabled, the wand instead.
	/// If no selection is active, will by default return false.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlapping(const Utilities::ivec2& _gridCoords) const;

	/// <summary>
	/// Returns whether we're hovering over any coordinate thats affected by
	/// the magic wand.
	/// If no selection is active, will by default return false.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlappingWand(const Utilities::ivec2& _gridCoords) const;

	/// <summary>
	/// Whether there's a wand selection active.
	/// </summary>
	/// <returns></returns>
	const bool IsWandActive() const;

	/// <summary>
	/// Whether we are overlapping anywhere within the selection box.
	/// If no selection is active, will by default return false.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlappingSelection(const Utilities::ivec2& _gridCoords) const;

	/// <summary>
	/// Whether this tile is selected.
	/// </summary>
	/// <returns></returns>
	const bool IsTileSelected(const Utilities::ivec2& _tile) const;
};