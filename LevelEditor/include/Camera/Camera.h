#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"

struct Camera 
{
	Utilities::ivec2 Position = { 0 };
	U32              Zoom     = 1;

	void Reset();

	Camera();
};