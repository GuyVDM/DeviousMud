#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class WorldEntity;

class Hitsplat : public UIComponent, public std::enable_shared_from_this<Hitsplat>
{
public:
	void set_follow_target(DM::Utils::UUID _targetId);

	void set_hit_amount(int32_t _hitAmount);

public:
	using UIComponent::UIComponent;

	virtual ~Hitsplat() = default;

	virtual void update() override; 

private:
	std::weak_ptr<WorldEntity> m_target;

	const float m_fadeDuration = 5.0f;
	float m_elapsedTime = 0;
};