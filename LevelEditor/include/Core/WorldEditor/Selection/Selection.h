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
	bool                          bLeftClicked = false;
	Utilities::ivec2              PointA  = { 0 };
	Utilities::ivec2              PointB  = { 0 };
	std::vector<Utilities::ivec2> WandSelectedTiles;

	bool                          bIsDragging    = false;
	Utilities::ivec2              startPointA    = { 0 };
	Utilities::ivec2              startPointB    = { 0 };
	Utilities::ivec2              startDragPos   = { 0 };
	std::vector<DragArgs>         selectedTiles;
	std::vector<Utilities::ivec2> StartWandSelectedTiles;

	void Reset();

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
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlapping(const Utilities::ivec2& _gridCoords) const;

	/// <summary>
	/// Returns whether we're hovering over any coordinate thats affected by
	/// the magic wand.
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
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	const bool IsOverlappingSelection(const Utilities::ivec2& _gridCoords) const;
};