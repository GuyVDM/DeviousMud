#include "precomp.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/HUDTab/HUDTab.h"

std::shared_ptr<HUDTab> HUDTab::create_tab(Utilities::vec2 _pos, Utilities::vec2 _scale, e_TabType _type, const Graphics::Sprite& _icon)
{
    //Grab the renderer
    const std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

    //Create tab instance.
    HUDTab* tab = new HUDTab(_pos, _scale, renderer->get_sprite(Graphics::SpriteType::HUD_TAB));
    tab->type = _type;
    tab->icon = _icon;

    return std::shared_ptr<HUDTab>(tab);
}

void HUDTab::on_left_click()
{
    on_click.invoke(type);
}

void HUDTab::on_selected(e_TabType _type)
{
    const static SDL_Color defaultColor = { 255, 255, 255, 255 };
    const static SDL_Color red     = { 255, 0, 0, 255 };

    sprite.color = _type == type ? red : defaultColor;
}

const Graphics::Sprite& HUDTab::get_icon()
{
    return icon;
}

HUDTab::HUDTab(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite) : UIComponent(_pos, _size, _sprite)
{
   
}
