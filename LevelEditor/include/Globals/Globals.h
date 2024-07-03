#pragma once
#include "Core/Core.hpp"

#include <memory>

struct Camera;
class Renderer;

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
typedef struct _Globals
{
	Ref<Renderer> Renderer;
	Ref<Camera>   Camera;
}   Globals;

extern Globals g_globals;