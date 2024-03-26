#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class HUDTabMenu : public UIComponent 
{
public:
	virtual ~HUDTabMenu() = default;

private:
	using UIComponent::UIComponent;

	virtual void init();
};