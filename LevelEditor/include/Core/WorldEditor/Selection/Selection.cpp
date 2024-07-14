#include "precomp.h"

#include "Core/WorldEditor/Selection/Selection.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/Config/Config.h"

void SelectionArgs::MoveSelectionRelativeTo(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 offset = _gridCoords - startDragPos;

	PointA = startPointA + offset;
	PointB = startPointB + offset;

	for (U32 i = 0; i < WandSelectedTiles.size(); i++)
	{
		WandSelectedTiles[i] = StartWandSelectedTiles[i] + offset;
	}

	for (DragArgs& dragArgs : SelectedTiles)
	{
		Utilities::ivec2 newGridPos = dragArgs.StartGridPos + offset;

		const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(newGridPos);
		const Utilities::ivec2 chunkCoords      = Chunk::ToChunkCoords(newGridPos);

		dragArgs.Tile->ChunkCoords      = chunkCoords;
		dragArgs.Tile->LocalChunkCoords = localChunkCoords;
	}
}

const std::vector<Utilities::ivec2> SelectionArgs::GetAffectedTiles() const
{
	if (IsWandActive())
	{
		return WandSelectedTiles;
	}

	const Utilities::ivec2 min = { std::min<I32>(PointA.x, PointB.x),
								   std::min<I32>(PointA.y, PointB.y)};

	const Utilities::ivec2 max = { std::max<I32>(PointA.x, PointB.x),
								   std::max<I32>(PointA.y, PointB.y)};

	std::vector<Utilities::ivec2> affectedTiles;
	for (I32 x = min.x; x <= max.x; x++)
	{
		for (I32 y = min.y; y <= max.y; y++)
		{
			affectedTiles.push_back(Utilities::ivec2(x, y));
		}
	}

	return affectedTiles;
}

const bool SelectionArgs::IsOverlapping(const Utilities::ivec2& _gridCoords) const
{
	if(!bIsActive) 
	{
		return false;
	}

	if (IsWandActive())
	{
		return IsOverlappingWand(_gridCoords);
	}

	return IsOverlappingSelection(_gridCoords);
}

const bool SelectionArgs::IsOverlappingWand(const Utilities::ivec2& _gridCoords) const
{
	if(!bIsActive) 
	{
		return false;
	}

	for (const Utilities::ivec2& pos : WandSelectedTiles)
	{
		if (_gridCoords == pos)
		{
			return true;
		}
	}

	return false;
}

const bool SelectionArgs::IsWandActive() const
{
	return WandSelectedTiles.size() > 0;
}

const bool SelectionArgs::IsOverlappingSelection(const Utilities::ivec2& _gridCoords) const
{
	if(!bIsActive) 
	{
		return false;
	}

	//If magic wand is inactive, we just check the selection field if there's overlap there.
	const Utilities::ivec2 min = { std::min(PointA.x, PointB.x), std::min(PointA.y, PointB.y) };
	const Utilities::ivec2 max = { std::max(PointA.x, PointB.x), std::max(PointA.y, PointB.y) };

	return (_gridCoords.x >= min.x && _gridCoords.x <= max.x && _gridCoords.y >= min.y && _gridCoords.y <= max.y);
}
