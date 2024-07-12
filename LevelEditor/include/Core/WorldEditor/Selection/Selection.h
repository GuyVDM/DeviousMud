#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"

struct SelectionArgs 
{
	bool bIsActive    = false;
	bool bLeftClicked = false;
	Utilities::ivec2 PointA  = { 0 };
	Utilities::ivec2 PointB  = { 0 };

	inline const bool IsPointOverlapping(const Utilities::ivec2& _point) 
	{
		const Utilities::ivec2 min = { std::min(PointA.x, PointB.x), std::min(PointA.y, PointB.y) };
		const Utilities::ivec2 max = { std::max(PointA.x, PointB.x), std::max(PointA.y, PointB.y) };

		return (_point.x >= min.x && _point.x <= max.x && _point.y >= min.y && _point.y <= max.y);
	}
};