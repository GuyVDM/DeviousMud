#include "precomp.h"

#include "Core/Input/InputHandler/InputHandler.h"

#include "Core/Camera/Camera.h"
#include "Core/Globals/Globals.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/Input/Input.h"

void InputHandler::ExtractInput(const SDL_Event& _event)
{
    switch(_event.type) 
    {
        case SDL_KEYDOWN:
        {
            const bool bIsRepeating = _event.key.repeat != 0;
            if (bIsRepeating)
            {
                return;
            }

            SDL_KeyCode key = static_cast<SDL_KeyCode>(_event.key.keysym.sym);
            g_Input->SetKeyState(key, true);
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            const e_MouseButton button = static_cast<e_MouseButton>(_event.button.button);
            g_Input->SetMouseState(button, true);
        }
        break;

        case SDL_KEYUP:
        {
            SDL_KeyCode key = static_cast<SDL_KeyCode>(_event.key.keysym.sym);
            g_Input->SetKeyState(key, false);
        }
        break;

        case SDL_MOUSEBUTTONUP:
        {
            const e_MouseButton button = static_cast<e_MouseButton>(_event.button.button);
            g_Input->SetMouseState(button, false);
        }
        break;
    }
}

void InputHandler::Update()
{
    g_Input->Update();
}

