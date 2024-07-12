#include "precomp.h"

#include "Core/Layers/InputLayer/InputLayer.h"

#include "Core/Camera/Camera.h"
#include "Core/Globals/Globals.h"
#include "Core/WorldEditor/WorldEditor.h"

InputLayer::InputLayer()
{
    m_WorldEditor = g_globals.WorldEditor;
}

const bool InputLayer::HandleEvent(const SDL_Event& _event)
{
    static bool bHoldingMiddleMouse = false;
    static bool bLeftMouse  = false;
    static bool bRightMouse = false;
    static bool bCtrlPressed = false;

    if (bLeftMouse)
    {
        m_WorldEditor->Place();
    }

    if(bRightMouse) 
    {
        if (!bCtrlPressed)
        {
            m_WorldEditor->Remove();
        }
    }

    switch(_event.type) 
    {
        case SDL_KEYDOWN:
        {
            if(_event.key.keysym.sym == SDLK_LCTRL || _event.key.keysym.sym == SDLK_RCTRL)
            {
                bCtrlPressed = true;
            }
        }
        break;

        case SDL_KEYUP:
        {
            if (_event.key.keysym.sym == SDLK_LCTRL || _event.key.keysym.sym == SDLK_RCTRL)
            {
                bCtrlPressed = false;
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (_event.button.button == SDL_BUTTON_LEFT)
                bLeftMouse = true;
            

            if (_event.button.button == SDL_BUTTON_RIGHT)
                bRightMouse = true;
            

            if (_event.button.button == SDL_BUTTON_MIDDLE)
                bHoldingMiddleMouse = true;       
        }
        break;

        case SDL_MOUSEBUTTONUP:
        {
            if (_event.button.button == SDL_BUTTON_LEFT) 
            {
                m_WorldEditor->Release();
                bLeftMouse = false;
            }


            if (_event.button.button == SDL_BUTTON_RIGHT)
                bRightMouse = false;


            if (_event.button.button == SDL_BUTTON_MIDDLE)
                bHoldingMiddleMouse = false;
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

