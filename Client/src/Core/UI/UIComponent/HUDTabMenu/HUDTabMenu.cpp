#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/HUDTabMenu/HUDTabMenu.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

using namespace Utilities;
using namespace Graphics;

void HUDTab::init()
{
	on_render_call.add_listener
	(
		std::bind(&HUDTab::renderBG, this, std::placeholders::_1)
	);

	set_icon(SpriteType::NONE, vec2(0.0f));
}

void HUDTab::set_icon(SpriteType _sprite, vec2 _iconSize)
{
	auto renderer = g_globals.renderer.lock();
	m_icon = renderer->get_sprite(_sprite);
	m_icon.zRenderPriority = m_sprite.zRenderPriority + 10;
	m_iconSize = _iconSize;
}

bool HUDTab::handle_event(const SDL_Event* _event) 
{
	if (is_hovered())
	{
		switch (_event->type)
		{
		case SDL_MOUSEBUTTONDOWN:
		{
			if (_event->button.button == SDL_BUTTON_RIGHT)
			{
				OptionArgs optionArgs;
				optionArgs.actionStr = "Open";
				optionArgs.actionCol = { 255, 255, 0, 255 };

				optionArgs.subjectStr = "Tab";
				optionArgs.subjectCol = { 229, 165, 80, 255 };
				optionArgs.function = std::bind(&HUDTab::on_left_click, this);

				OptionsTab::add_option(optionArgs);
			}
		}
		break;
		}
	}

	return Clickable::handle_event(_event);
}

void HUDTab::renderBG(std::shared_ptr<Renderer> _renderer)
{
	const Utilities::vec2 m_position; 

	const Utilities::vec2 sizeDiff = get_size() - m_iconSize;

	_renderer->plot_raw_frame(m_icon, get_position() + (sizeDiff / 2.0f), m_iconSize);
}

void HUDTab::on_left_click()
{
	m_sprite.color = { 255, 0, 0, 255};
}

void HUDTabMenu::on_new_parent()
{
}

/// <summary>
/// MAIN
/// </summary>
void HUDTabMenu::init()
{
	m_bIsMovable = true;

	float scale = 0.625f;
	const Size tabSize  = Utilities::vec2(84.0f) * scale;
	const Size m_iconSize = Utilities::vec2(64.0f) * scale;
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
		tab->set_icon(SpriteType::HUD_ICON_COMBAT, m_iconSize);
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
		tab->set_icon(SpriteType::HUD_ICON_SKILLS, m_iconSize);
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
		tab->set_icon(SpriteType::HUD_ICON_INVENTORY, m_iconSize);
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
		component->get_sprite().bIsFlipped = true;
		add_child(component);
	}
}
