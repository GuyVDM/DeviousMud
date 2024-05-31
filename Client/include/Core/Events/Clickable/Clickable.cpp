#include "precomp.h"
#include "Clickable.h"

#include "Shared/Utilities/vec2.hpp"

#include "Core/Network/Client/ClientWrapper.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

Clickable::Clickable(const Utilities::vec2& _pos, const Utilities::vec2& _scale, Graphics::Sprite _sprite) : EventReceiver(),
    m_pos(_pos), size(_scale), m_sprite(_sprite), m_bIsHovered(false)
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
                if (!m_bIsHovered) //If not hovering before, start hovering.
                {
                    m_bIsHovered = true;
                    on_hover();
                }

                return true;
            }

            if(m_bIsHovered) //If hovering before, but not anymore, stop hovering
            {
                m_bIsHovered = false;
                on_hover_end();
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
    const Utilities::ivec2 camPos = Utilities::to_ivec2(g_globals.renderer.lock()->get_camera()->get_position());

    Utilities::ivec2 viewportSize;
    g_globals.renderer.lock()->get_viewport_size(&viewportSize.x, &viewportSize.y);
    viewportSize.x /= 2;
    viewportSize.y /= 2;

    // Calculate sprite's screen coordinates
    const Utilities::ivec2 halfExtends = Utilities::to_ivec2(get_size() / 2.0f);

    const Utilities::ivec2 transformedPos = Utilities::to_ivec2(get_position() * Graphics::Renderer::GRID_CELL_PX_SIZE) - camPos
        + (viewportSize + halfExtends);

    // Calculate sprite's bounding box
    const int32_t left = transformedPos.x - halfExtends.x;
    const int32_t right = transformedPos.x + halfExtends.x;
    const int32_t top = transformedPos.y - halfExtends.y;
    const int32_t bottom = transformedPos.y + halfExtends.y;

    return (_x > left && _x < right && _y > top && _y < bottom);
}

void Clickable::on_left_click()
{
}

void Clickable::on_hover()
{
}

void Clickable::on_hover_end()
{
}

void Clickable::on_right_click()
{
}

const bool Clickable::is_hovered() const
{
    return m_bIsHovered;
}

Graphics::Sprite& Clickable::get_sprite()
{
    return m_sprite;
}

const Utilities::vec2 Clickable::get_position() const
{
    return m_pos;
}

const Utilities::vec2 Clickable::get_size() const
{
    return size;
}

void Clickable::set_position(Utilities::vec2 _pos)
{
    m_pos = _pos;
}

void Clickable::set_size(Utilities::vec2 _size)
{
    size = _size;
}

