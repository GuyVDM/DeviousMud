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

	const Utilities::vec2 position; 

	_renderer->plot_raw_frame(icon, get_position() + (iconSize / 2.0f), iconSize);
}

void HUDTab::on_left_click()
{
	sprite.color = { 255, 0, 0, 255};
}

/// <summary>
/// MAIN
/// </summary>
void HUDTabMenu::init()
{
	bIsMovable = true;

	float scale = 0.625f;
	const Size tabSize  = Utilities::vec2(84.0f) * scale;
	const Size iconSize = Utilities::vec2(42.0f) * scale;
	std::shared_ptr<HUDTab> tab;
	Position tabPos;
	
	// Inventory tab;
	{
		tabPos = get_position() - Utilities::vec2(tabSize.x / 2.0f, tabSize.y);
		tab = UIComponent::create_component<HUDTab>
			(
				tabPos, 
				tabSize, 
				SpriteType::HUD_TAB,
				true
			);
		tab->set_anchor(e_AnchorPreset::CENTER);
		tab->set_icon(SpriteType::HUD_ICON_SKILLS, iconSize);
		add_child(tab);
	}

	// PLACEHOLDER-002 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 0.5f, -tabSize.y);
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

	// PLACEHOLDER-003 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 1.5f, -tabSize.y);
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

	// PLACEHOLDER-004 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 2.5f, -tabSize.y);
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

	// PLACEHOLDER-005 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 3.5f, -tabSize.y);
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

	// PLACEHOLDER-006 tab;
	{
		tabPos = get_position() - Utilities::vec2(tabSize.x / 2.0f, -get_size().y);
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

	// PLACEHOLDER-007 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 0.5f, get_size().y);
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

	// PLACEHOLDER-008 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 1.5f, get_size().y);
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

	// PLACEHOLDER-009 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 2.5f, get_size().y);
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

	// PLACEHOLDER-010 tab;
	{
		tabPos = get_position() + Utilities::vec2(tabSize.x * 3.5f, get_size().y);
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

	std::shared_ptr<UIComponent> component;

	// Pillar-001;
	{
		component = UIComponent::create_component<UIComponent>
			(
				get_position() - Position(tabSize.x / 2.0f, 0.0f),
				Size(42.0f, 504.0f) * scale,
				SpriteType::HUD_FRAME
			);
		component->set_anchor(e_AnchorPreset::CENTER);
		add_child(component);
	}

	// Pillar-001;
	{
		std::shared_ptr<UIComponent> component;
		component = UIComponent::create_component<UIComponent>
			(
				get_position() + Position(get_size().x, 0.0f),
				Size(42.0f, 504.0f) * scale,
				SpriteType::HUD_FRAME
			);
		component->set_anchor(e_AnchorPreset::CENTER);
		add_child(component);
	}
}
