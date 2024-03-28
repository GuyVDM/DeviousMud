#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class HUDTab : public UIComponent
{
public:
	virtual ~HUDTab() = default;

	void set_icon(Graphics::SpriteType _sprite, Utilities::vec2 _iconSize);

	virtual void render(std::shared_ptr<Graphics::Renderer> _renderer) override;

	virtual void on_left_click() override;

private:
	virtual void init();

private:
	using UIComponent::UIComponent;

private:
	Graphics::Sprite icon;
	Utilities::vec2  iconSize;
};

/// <summary>
/// The Super class implementing all of the HUD Components.
/// </summary>
class HUDTabMenu : public UIComponent 
{
public:
	virtual ~HUDTabMenu() = default;

private:
	using UIComponent::UIComponent;

	virtual void init();
};