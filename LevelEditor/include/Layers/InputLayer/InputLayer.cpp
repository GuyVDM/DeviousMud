#include "precomp.h"

#include "Layers/InputLayer/InputLayer.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "WorldEditor/WorldEditor.h"

InputLayer::InputLayer()
{
    m_WorldEditor = std::make_shared<WorldEditor>();
}

const bool InputLayer::HandleEvent(const SDL_Event& _event)
{
    static bool bHoldingMiddleMouse = false;

    switch(_event.type) 
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            if (_event.button.button == SDL_BUTTON_MIDDLE)
            {
                bHoldingMiddleMouse = true;
            }
        }
        break;

        case SDL_MOUSEBUTTONUP:
        {
            if (_event.button.button == SDL_BUTTON_MIDDLE)
            {
                bHoldingMiddleMouse = false;
            }
        }
        break;

        case SDL_MOUSEMOTION:
        {
            if (bHoldingMiddleMouse)
            {
                Ref<Camera> camera = g_globals.Camera;

                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
                {
                    camera->Position.x -= _event.motion.xrel;
                    camera->Position.y -= _event.motion.yrel;
                }
            }
        }
        break;

        case SDL_MOUSEWHEEL:
        {
            Ref<Camera> camera = g_globals.Camera;
            camera->Zoom = std::clamp<U32>(camera->Zoom + _event.wheel.y, 1, 10);
        }
        break;
    }

	return false;
}

void InputLayer::Update()
{
    m_WorldEditor->Update();
}

