#pragma once
#include <memory>

#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/UI/UIComponent/HUDTab/HUDTabType.h"

#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/vec2.hpp"

class HUDTab final : public UIComponent 
{
public:
	/// <summary>
	/// Returns a new instance of a tab.
	/// </summary>
	/// <param name="_pos"></param>
	/// <param name="_scale"></param>
	/// <param name="_type"></param>
	/// <param name="_icon"></param>
	/// <returns></returns>
	static std::shared_ptr<HUDTab> create_tab(Utilities::vec2 _pos, Utilities::vec2 _scale, e_TabType _type, const Graphics::Sprite& _icon);

	/// <summary>
	/// Events get called whenever this button gets clicked.
	/// </summary>
	EventListener<e_TabType> on_click;

	/// <summary>
	/// Returns the details of the icon owned by this tab.
	/// </summary>
	/// <returns></returns>
	const Graphics::Sprite& get_icon();

	/// <summary>
	/// Gets called whenever a new tab has been selected.
	/// </summary>
	/// <param name="_type"></param>
	void on_selected(e_TabType _type);

public:
	virtual ~HUDTab() = default;

private:
	HUDTab(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);

	virtual void on_left_click();

	friend std::shared_ptr<HUDTab> create_tab(Utilities::vec2 _pos, Utilities::vec2 _scale, e_TabType _type, const Graphics::Sprite& _icon);

private:
	e_TabType        type;
	Graphics::Sprite icon;
};