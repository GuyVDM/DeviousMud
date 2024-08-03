#pragma once
#include "Core/Core.h"

#include "Shared/Utilities/vec2.hpp"

struct Camera 
{
	Utilities::ivec2 Position = { 0 };
	I32              Zoom     = 1;

	void Reset();

	Camera();
};