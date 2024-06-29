#include "precomp.h"

#include "Core/Entity/WorldEntity/States/WorldEntityStates.h"

#include "Core/Entity/WorldEntity/WorldEntity.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

void EntityWalkingState::on_state_start()
{
	RefEntity owner = m_owner.lock();

	using namespace Graphics::Animation;

	const NPCDef& def = owner->get_definition();
	Animator::play_animation(owner->get_sprite(), def.walkingAnim, true, 10.0f);
}

void EntityWalkingState::on_state_update()
{
	RefEntity owner = m_owner.lock();

	if (owner->m_bIsDead)
	{
		owner->set_state(e_EntityState::STATE_DEAD);
		return;
	}

	if (!owner->m_simPos.is_dirty())
	{
		owner->set_state(e_EntityState::STATE_IDLE);
		return;
	}

	owner->m_simPos.update();

}

void EntityWalkingState::on_state_end() 
{
}

void EntityIdleState::on_state_start()
{
	RefEntity owner = m_owner.lock();

	using namespace Graphics::Animation;

	const NPCDef& def = owner->get_definition();
	Animator::play_animation(owner->get_sprite(), def.idleAnim, true, 10.0f);
}

void EntityIdleState::on_state_update()
{
	RefEntity owner = m_owner.lock();

	if (owner->m_bIsDead)
	{
		owner->set_state(e_EntityState::STATE_DEAD);
		return;
	}

	if(owner->m_simPos.is_dirty()) 
	{
		owner->set_state(e_EntityState::STATE_WALKING);
	}
}

void EntityIdleState::on_state_end()
{
}

void EntityAttackingState::on_state_start()
{
	RefEntity owner = m_owner.lock();

	using namespace Graphics::Animation;

	const NPCDef& def = owner->get_definition();
	Animator::play_animation(owner->get_sprite(), def.attackAnim, false, 10.0f);
}

void EntityAttackingState::on_state_update()
{
	RefEntity owner = m_owner.lock();

	if (owner->m_bIsDead)
	{
		owner->set_state(e_EntityState::STATE_DEAD);
		return;
	}

	using namespace Graphics::Animation;
	if(!Animator::is_playing(owner->get_sprite(), owner->m_npcDefinition.attackAnim)) 
	{
		//*--------------------------------------------------------------------------
		// After attacking, make entity walk again if there's movement to catch up to.
		//*
		if(owner->m_simPos.is_dirty()) 
		{
			owner->set_state(e_EntityState::STATE_WALKING);
			return;
		}

		owner->set_state(e_EntityState::STATE_IDLE);
	}
}

void EntityAttackingState::on_state_end()
{
}

void EntityDeathState::on_state_start()
{
	RefEntity owner = m_owner.lock();

	using namespace Graphics::Animation;

	const NPCDef& def = owner->get_definition();
	Animator::play_animation_oneshot(owner->get_sprite(), def.deathAnim, 10.0f);
}

void EntityDeathState::on_state_update()
{
	RefEntity owner = m_owner.lock();

	if(!owner->m_bIsDead) 
	{
		owner->set_state(e_EntityState::STATE_IDLE);
		return;
	}
}

void EntityDeathState::on_state_end()
{
}
