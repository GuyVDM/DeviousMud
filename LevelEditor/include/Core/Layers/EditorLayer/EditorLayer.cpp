#include "precomp.h"

#include "Core/Layers/EditorLayer/EditorLayer.h"

#include "Core/WorldEditor/WorldEditor.h"
#include "Core/Camera/Camera.h"
#include "Core/Globals/Globals.h"
#include "Core/Input/Input.h"

const bool EditorLayer::HandleEvent(const SDL_Event& _event)
{
    //*----------------------------------------------------
    // Handle main inputs here as we want other layers to block
    // this input when needed.
    {
        if (g_Input->GetMouseUp(e_MouseButton::BUTTON_LEFT))
        {
            g_globals.WorldEditor->Release();
        }

        if (g_Input->GetMouse(e_MouseButton::BUTTON_LEFT))
        {
            g_globals.WorldEditor->Place();
        }

        if (g_Input->GetMouse(e_MouseButton::BUTTON_RIGHT))
        {
            static bool bCtrlPressed = g_Input->GetKey(SDLK_LCTRL) || g_Input->GetKey(SDLK_RCTRL);

            if (!bCtrlPressed)
            {
                g_globals.WorldEditor->Remove();
            }
        }

        if (g_Input->GetMouseUp(e_MouseButton::BUTTON_LEFT))
        {
            g_globals.WorldEditor->Release();
        }
    }

    switch(_event.type) 
    {
        case SDL_MOUSEMOTION:
        {
            if (g_Input->GetMouse(e_MouseButton::BUTTON_MIDDLE))
            {
                Ref<Camera> camera = g_globals.Camera;

                if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
                {
                    camera->Position.x -= _event.motion.xrel;
                    camera->Position.y -= _event.motion.yrel;
                }
            }
        }
        return true;

        case SDL_MOUSEWHEEL:
        {
            Ref<Camera> camera = g_globals.Camera;
            camera->Zoom = std::clamp<U32>(camera->Zoom + _event.wheel.y, 1, 10);
        }
        return true;
    }

    return false;
}

void EditorLayer::Update()
{
    g_globals.WorldEditor->Update();
}
