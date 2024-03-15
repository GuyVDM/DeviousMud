#include "precomp.h"

#include <limits>

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/UIComponent/HUDTab/HUDTab.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Renderer.h"

using namespace Utilities;

void Graphics::UI::HUDLayer::init()
{
	create_hud();
}

//vec2 Graphics::UI::HUDLayer::get_hud_size()
//{
//	float left  = std::numeric_limits<float>::max();
//	float up    = std::numeric_limits<float>::max();
//	float right = 0;
//	float down  = 0;
//
//	//for(const auto& element : elements) 
//	//{
//	//	if (element.position.x < left)
//	//		left = element.position.x;
//
//	//	if (right < element.position.x + element.scale.x)
//	//		right = element.position.x + element.scale.x;
//
//	//	if (up > element.position.y)
//	//		up = element.position.y;
//
//	//	if (down < element.position.y + element.scale.y)
//	//		down = element.position.y + element.scale.y;
//	//}
//
//	//for(const auto& tab : tabs)
//	//{
//	//	if (tab->get_pos().x < left)
//	//		left = tab->get_pos().x;
//
//	//	if (right < tab->get_pos().x + tab->get_size().x)
//	//		right = tab->get_pos().x + tab->get_size().x;
//
//	//	if (up > tab->get_pos().y)
//	//		up = tab->get_pos().y;
//
//	//	if (down < tab->get_pos().y + tab->get_size().y)
//	//		down = tab->get_pos().y + tab->get_size().y;
//	//}
//
//	return vec2();
//}

void Graphics::UI::HUDLayer::create_hud()
{
	const vec2 interfacePos = vec2(0.0f, 100.0f);
	
	const float hudScale = 0.5f;

	const auto scaledToHud = [&hudScale](float _item)
		{
			return _item * hudScale;
		};

#pragma region TABS
	std::shared_ptr<HUDTab> tab;
	Graphics::Sprite icon;

	vec2 tabSize = renderer->get_sprite(Graphics::SpriteType::HUD_TAB).get_dimensions();
	tabSize.x = scaledToHud(tabSize.x);
	tabSize.y = scaledToHud(tabSize.y);
	
	const float tabSpacing = tabSize.y * 7.0f;

	// Skills
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_SKILLS);
		tab = HUDTab::create_tab(vec2(interfacePos.x, interfacePos.y), tabSize, e_TabType::SKILLS, icon);
	}	tabs.push_back(tab);

	// Quests
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x, interfacePos.y), tabSize, e_TabType::QUESTS, icon);
	}	tabs.push_back(tab);

	// Inventory
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 2, interfacePos.y), tabSize, e_TabType::INVENTORY, icon);
	}	tabs.push_back(tab);

	// Equipment
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 3, interfacePos.y), tabSize, e_TabType::EQUIPMENT, icon);
	}	tabs.push_back(tab);

	// Prayer
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 4, interfacePos.y), tabSize, e_TabType::PRAYER, icon);
	}	tabs.push_back(tab);

	// Placeholder 6
	{;
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x, interfacePos.y + tabSpacing), tabSize, e_TabType::PLACEHOLDER_06, icon);

	}	tabs.push_back(tab);

	// Placeholder 7
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x, interfacePos.y + tabSpacing), tabSize, e_TabType::PLACEHOLDER_07, icon);

	}	tabs.push_back(tab);

	// Placeholder 8
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 2, interfacePos.y + tabSpacing), tabSize, e_TabType::PLACEHOLDER_08, icon);

	}	tabs.push_back(tab);

	// Placeholder 9
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 3, interfacePos.y + tabSpacing), tabSize, e_TabType::PLACEHOLDER_09, icon);

	}	tabs.push_back(tab);

	// Placeholder 10
	{
		icon = renderer->get_sprite(SpriteType::HUD_ICON_PLACEHOLDER);
		tab = HUDTab::create_tab(vec2(interfacePos.x + tabSize.x * 4, interfacePos.y + tabSpacing), tabSize, e_TabType::PLACEHOLDER_10, icon);

	}	tabs.push_back(tab);
#pragma endregion

#pragma region NON_INTERACTABLE_UI
	UIElement item;
	// Backdrop for tab interfaces.
	{
		item.sprite     = renderer->get_sprite(SpriteType::HUD_BACKDROP);
		item.position.x = interfacePos.x + scaledToHud(tab->get_sprite().get_width()) * 0.5f;
		item.position.y = interfacePos.y + scaledToHud(tab->get_sprite().get_height());
		item.scale      = vec2(item.sprite.get_width(), item.sprite.get_height()) * hudScale;
	}	elements.push_back(item);

	// Side frames for each side of the backdrop.
	{
		item.sprite = renderer->get_sprite(SpriteType::HUD_FRAME);
		item.position.x = interfacePos.x;
		item.position.y = interfacePos.y + scaledToHud(tab->get_sprite().get_height());
		item.scale      = vec2(item.sprite.get_width(), item.sprite.get_height()) * hudScale;
	}	elements.push_back(item);

	// Side frames for each side of the backdrop.
	{
		item.sprite = renderer->get_sprite(SpriteType::HUD_FRAME);
		item.position.x = interfacePos.x + scaledToHud(tab->get_sprite().get_width()) * 4.5f;
		item.position.y = interfacePos.y + scaledToHud(tab->get_sprite().get_height());
		item.scale = vec2(item.sprite.get_width(), item.sprite.get_height()) * hudScale;
	}	elements.push_back(item);

#pragma endregion

	// Bind all events.
	for(const auto& tab : tabs) 
	{
		tab->on_click.add_listener
		(
			std::bind(&HUDLayer::open_menu, this, std::placeholders::_1)
		);

		on_selected.add_listener
		(
			std::bind(&HUDTab::on_selected, tab, std::placeholders::_1)
		);
	}
}

void Graphics::UI::HUDLayer::open_menu(e_TabType _tab)
{
	on_selected.invoke(_tab);
}

bool Graphics::UI::HUDLayer::handle_event(const SDL_Event* event)
{
	for (const auto& tab : tabs)
	{
		if(tab->handle_event(event)) 
		{
			return true;
		}
	}

	return false;
}

void Graphics::UI::HUDLayer::update()
{
	// Render all tabs
	for(const auto& tab : tabs) 
	{
		const Sprite& icon = tab->get_icon();

		renderer->plot_raw_frame(tab->get_sprite(), tab->get_pos(), tab->get_size());
		renderer->plot_raw_frame(icon, tab->get_pos(), tab->get_size());
	}

	for(const auto& element : elements) 
	{
		renderer->plot_raw_frame(element.sprite, element.position, element.scale);
	}
}
