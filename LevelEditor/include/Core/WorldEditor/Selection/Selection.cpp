#include "precomp.h"

#include "Core/WorldEditor/Selection/Selection.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/Config/Config.h"

const bool SelectionArgs::IsEmpty() const
{
	return SelectedTiles.size() == 0;
}

void SelectionArgs::AddTileToSelection(const Utilities::ivec2& _gridCoords)
{
	if (!IsOverlapping(_gridCoords))
	{
		SelectedTiles.push_back(_gridCoords);
	}
}

void SelectionArgs::RemoveTileFromSelection(const Utilities::ivec2& _gridCoords)
{
	auto it = std::find(SelectedTiles.begin(), SelectedTiles.end(), _gridCoords);

	if(it != SelectedTiles.end()) 
	{
		SelectedTiles.erase(it);
	}
}

void SelectionArgs::MoveSelectionRelativeTo(const Utilities::ivec2& _gridCoords)
{
	const Utilities::ivec2 offset = _gridCoords - StartDragPos;

	for(U32 i = 0; i < SelectedTiles.size(); i++) 
	{
		SelectedTiles[i] = SelectedTilesStart[i] + offset;
	}

	for (DragArgs& dragArgs : SelectedDraggingTiles)
	{
		Utilities::ivec2 newGridPos = dragArgs.StartGridPos + offset;

		const Utilities::ivec2 localChunkCoords = Chunk::ToLocalChunkCoords(newGridPos);
		const Utilities::ivec2 chunkCoords      = Chunk::ToChunkCoords(newGridPos);

		dragArgs.Tile->ChunkCoords      = chunkCoords;
		dragArgs.Tile->LocalChunkCoords = localChunkCoords;
	}
}

const std::vector<Utilities::ivec2> SelectionArgs::GetSelectedGridSpaces() const
{
	return SelectedTiles;
}

const bool SelectionArgs::IsOverlapping(const Utilities::ivec2& _gridCoords) const
{
	auto it = std::find(SelectedTiles.begin(), SelectedTiles.end(), _gridCoords);
	return it != SelectedTiles.end();
}

const bool SelectionField::IsActive() const
{
	return bIsActive;
}

void SelectionField::GetFromTo(Utilities::ivec2& _from, Utilities::ivec2& _to) const
{
	_from = PointA;
	_to   = PointB;
}

bool SelectionField::Begin(const Utilities::ivec2& _a)
{
	if (!bIsActive)
	{
		PointA = _a;
		bIsActive = true;
		return true;
	}

	return false;
}

void SelectionField::DrawRectTo(const Utilities::ivec2& _b)
{
	if(bIsActive) 
	{
		PointB = _b;
	}
}

std::vector<Utilities::ivec2> SelectionField::GetTilesAndCollapse()
{
	const Utilities::ivec2 min = { std::min<I32>(PointA.x, PointB.x),
							       std::min<I32>(PointA.y, PointB.y)};

	const Utilities::ivec2 max = { std::max<I32>(PointA.x, PointB.x),
								   std::max<I32>(PointA.y, PointB.y)};

	const U32 width  = static_cast<U32>(abs(max.x - min.x + 1));
	const U32 height = static_cast<U32>(abs(max.y - min.y + 1));
	const U32 tileCount = width * height;

	std::vector<Utilities::ivec2> Tiles(tileCount);

	for (I32 x = min.x; x <= max.x; x++)
	{
		for (I32 y = min.y; y <= max.y; y++)
		{
			const I32 i = (x - min.x) + width * (y - min.y);

			Tiles[i] = Utilities::ivec2(x, y);
		}
	}

	PointA    = { 0, 0 };
	PointB    = { 0, 0 };
	bIsActive  = false;

	return Tiles;
}
