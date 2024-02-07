#include "precomp.h"
#include "Clickable.h"

#include "Shared/Utilities/vec2.hpp"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

Clickable::Clickable(const Utilities::vec2& _size, const Utilities::vec2& _pos, Graphics::Sprite _sprite) : EventReceiver(),
    pos(_pos), size(_size), sprite(_sprite), is_hovered(false)
{
}

Clickable::~Clickable()
{
    //Ewww..
    //I don't think i need to create a texture from a pre-existing surface, it's fine for members to share.
    g_globals.renderer.lock()->destroy_sprite(sprite);
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
                if (!is_hovered) //If not hovering before, start hovering.
                {
                    is_hovered = true;
                    on_hover();
                }

                return true;
            }

            if(is_hovered) //If hovering before, but not anymore, stop hovering
            {
                is_hovered = false;
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
    return (pos.x < _x) && (pos.x + size.x > _x) && //Is within the horizontal boundries of the rect.
           (pos.y < _y) && (pos.y + size.y > _y);   //Is within the vertical boundries of the rect.
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

const Utilities::vec2& Clickable::get_pos() const
{
    return pos;
}

const Utilities::vec2& Clickable::get_size() const
{
    return size;
}