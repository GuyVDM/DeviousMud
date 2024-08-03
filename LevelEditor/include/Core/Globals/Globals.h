#pragma once
#include "Core/Core.h"

#include <memory>

struct Camera;
class Renderer;
class WorldEditor;

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
typedef struct _Globals
{
	Ref<Renderer>    Renderer;
	Ref<Camera>      Camera;
	Ref<WorldEditor> WorldEditor;
}   Globals;

extern Globals g_globals;