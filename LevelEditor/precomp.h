#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_keycode.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vendor/nolhmann/json.hpp>

#include "Shared/Utilities/Logger.hpp"
#include "Shared/Utilities/Assert.h"

#include "Core/Math/DMath.hpp"