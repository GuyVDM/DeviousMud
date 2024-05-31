#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Shared/Utilities/EventListener.h"

class WorldEntity;

class Healthbar : public UIComponent
{
public:
	static std::shared_ptr<Healthbar> create_hp_bar(Utilities::vec2 _size, std::shared_ptr<WorldEntity> _owner);

	virtual ~Healthbar() = default;

private:
	using UIComponent::UIComponent;

	virtual void init() override;

	void render_healthbar(std::shared_ptr<Graphics::Renderer> _renderer);

private:
	float m_fillAmount = 1.0f;
	std::weak_ptr<WorldEntity> m_owner;

};