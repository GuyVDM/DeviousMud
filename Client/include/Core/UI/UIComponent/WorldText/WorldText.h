#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class WorldEntity;

class WorldText : public UIComponent, public std::enable_shared_from_this<WorldText>
{
public:
	void set_follow_target(DM::Utils::UUID _targetId);

	void set_text(std::string _text);

public:
	using UIComponent::UIComponent;

	virtual ~WorldText() = default;

	virtual void update() override;

private:
	std::weak_ptr<WorldEntity> m_target;

	float       m_elapsedTime = 0.0f;
	const float m_displayDuration = 4.0f;
};