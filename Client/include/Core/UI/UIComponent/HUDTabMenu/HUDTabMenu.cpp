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

void HUDTab::on_left_click()
{
	sprite.color.r = 255;
}

/// <summary>
/// MAIN
/// </summary>
void HUDTabMenu::init()
{
	bIsMovable = true;

	const Size tabSize = Utilities::vec2(64.0f);
	std::shared_ptr<HUDTab> tab;
	Position tabPos;
	
	// Inventory tab;
	{
		tabPos = get_position() - Utilities::vec2(0.0f, tabSize.y);
		tab = UIComponent::create_component<HUDTab>
			(
				tabPos, 
				tabSize, 
				SpriteType::HUD_TAB,
				true
			);
		tab->set_anchor(e_AnchorPreset::CENTER);
		add_child(tab);
	}
}
