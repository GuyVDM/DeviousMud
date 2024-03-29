#include "precomp.h"
#include "Clickable.h"

#include "Shared/Utilities/vec2.hpp"

#include "Core/Network/Client/ClientWrapper.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

Clickable::Clickable(const Utilities::vec2& _pos, const Utilities::vec2& _scale, Graphics::Sprite _sprite) : EventReceiver(),
    pos(_pos), size(_scale), sprite(_sprite), bIsHovered(false)
{
}

Clickable::~Clickable()
{

}

bool Clickable::handle_event(const SDL_Event* _event)
{
    if (_event->type == SDL_MOUSEMOTION || _event->type == SDL_MOUSEBUTTONDOWN)
    {
        const int mouseX = _event->motion.x;
        const int mouseY = _event->motion.y;
        const bool is_overlapping = overlaps_rect(mouseX, mouseY);

        switch (_event->type)
        {
        case SDL_MOUSEMOTION:
        {
            if (is_overlapping)
            {
                if (!bIsHovered) //If not hovering before, start hovering.
                {
                    bIsHovered = true;
                    on_hover();
                }

                return true;
            }

            if(bIsHovered) //If hovering before, but not anymore, stop hovering
            {
                bIsHovered = false;
                on_stop_hover();
                return false;
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (is_overlapping)
            {
                switch (_event->button.button)
                {
                case SDL_BUTTON_LEFT:
                    on_left_click();
                    return true;

                case SDL_BUTTON_RIGHT:
                    on_right_click();
                    return true;
                }
            }
        }
        break;
        }
    }


    return false;
}

const bool Clickable::overlaps_rect(const int& _x, const int& _y) const
{
    Utilities::vec2 camPos = g_globals.renderer.lock()->get_camera()->get_position();

    int32_t viewportWidth, viewportHeight;
    g_globals.renderer.lock()->get_viewport_size(&viewportWidth, &viewportHeight);

    viewportWidth /= 2;
    viewportHeight /= 2;

    // Calculate sprite's screen coordinates
    int32_t halfExtendsWidth = static_cast<int32_t>(size.x / 2.0f);
    int32_t halfExtendsHeight = static_cast<int32_t>(size.y / 2.0f);

    Utilities::ivec2 transformedPos;
    transformedPos.x = (static_cast<int32_t>(pos.x) * Graphics::Renderer::GRID_CELL_PX_SIZE) - (int32_t)camPos.x + viewportWidth;
    transformedPos.y = (static_cast<int32_t>(pos.y) * Graphics::Renderer::GRID_CELL_PX_SIZE) - (int32_t)camPos.y + viewportHeight;
    transformedPos.x += halfExtendsWidth;
    transformedPos.y += halfExtendsHeight;

    // Calculate sprite's bounding box
    const int32_t left   = transformedPos.x - halfExtendsWidth;
    const int32_t right  = transformedPos.x + halfExtendsWidth;
    const int32_t top    = transformedPos.y - halfExtendsHeight;
    const int32_t bottom = transformedPos.y + halfExtendsHeight;

    return (_x > left && _x < right && _y > top && _y < bottom);
}

void Clickable::on_left_click()
{
}

void Clickable::on_hover()
{
}

void Clickable::on_stop_hover()
{
}

void Clickable::on_right_click()
{
}

const Graphics::Sprite& Clickable::get_sprite() const
{
    return sprite;
}

const Utilities::vec2& Clickable::get_position() const
{
    return pos;
}

const Utilities::vec2& Clickable::get_size() const
{
    return size;
}

void Clickable::set_position(Utilities::vec2 _pos)
{
    pos = _pos;
}

void Clickable::set_size(Utilities::vec2 _size)
{
    size = _size;
}

