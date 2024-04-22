#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class HUDTab : public UIComponent
{
public:
	virtual ~HUDTab() = default;

	void set_icon(Graphics::SpriteType _sprite, Utilities::vec2 _iconSize);

	virtual bool handle_event(const SDL_Event* _event) override;
protected:

	virtual void on_left_click() override;
	
	virtual void init() override;

private:
	void renderBG(std::shared_ptr<Graphics::Renderer> _renderer) ;

	using UIComponent::UIComponent;

private:
	Graphics::Sprite m_icon;
	Utilities::vec2  m_iconSize;
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