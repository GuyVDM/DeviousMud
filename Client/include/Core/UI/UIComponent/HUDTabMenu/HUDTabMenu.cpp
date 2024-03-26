#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/HUDTabMenu/HUDTabMenu.h"

using namespace Utilities;
using namespace Graphics;

void HUDTab::init()
{
	set_icon(SpriteType::NONE, vec2(0.0f));
}

void HUDTab::set_icon(SpriteType _sprite, vec2 _iconSize)
{
	auto renderer = g_globals.renderer.lock();
	icon = renderer->get_sprite(_sprite);
	iconSize = _iconSize;
}

void HUDTab::render(std::shared_ptr<Renderer> _renderer)
{
	UIComponent::render(_renderer);
}

/// <summary>
/// MAIN
/// </summary>
void HUDTabMenu::init()
{
	set_movable(true);
}
