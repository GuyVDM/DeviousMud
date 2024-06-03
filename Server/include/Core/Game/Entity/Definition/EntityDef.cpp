#include "precomp.h"

#include "Core/Game/Entity/Definition/EntityDef.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Game/Combat/CombatHandler.h"

#include <random>

void Entity::update() 
{
	if(tickCounter > 0) 
	{
		tickCounter--;
	}

	tick();
}

void Entity::tick()
{
	/// Override in child classes.
	///
	/// 
}

void Entity::disengage() 
{

}

void Entity::try_set_target(std::shared_ptr<Entity> _entity, bool _bWasInstigated)
{

}

int Entity::get_attack_range()
{
	return 0;
}

int Entity::get_attack_speed()
{
	return 3;
}

/// <summary>
/// TODO: Replace these with statemachines maybe?
/// </summary>
void NPC::tick()
{
	if(auto target = m_target.lock(); !m_target.expired()) 
	{
		int32_t distance = Utilities::ivec2::get_distance(startingPosition, position);

		if(Utilities::ivec2::get_distance(position, target->position) > static_cast<int32_t>(maxWanderingDistance)) 
		{
			m_target.reset();
		}
		else
		{
			auto optNPC = g_globals.entityHandler->get_entity(uuid);

			CombatHandler::engage
			(
				std::dynamic_pointer_cast<NPC>(optNPC.value()),
				target
			);
			return;
		}
	}

	//*
	// Movement behaviour of NPC's, they roll a dice 1-9 every game tick.
	// If the roll was succesfull, the npc will choose a random location within its
	// specified wandering range.
	//*
	{
		if (!m_bIsMoving)
		{
			const int chanceToMove = 9;
			const int dice = (rand() % chanceToMove) + 1;

			if (dice == chanceToMove)
			{
				Utilities::ivec2 offset = Utilities::ivec2
				(
					(rand() % wanderingDistance) - (wanderingDistance / 2),
					(rand() % wanderingDistance) - (wanderingDistance / 2)
				);

				m_bIsMoving = true;

				m_targetPos = startingPosition + offset;
			}
		}

		if (m_bIsMoving)
		{
			g_globals.entityHandler->move_entity_to(uuid, m_targetPos);

			if (m_targetPos == position)
			{
				m_bIsMoving = false;
			}
		}
	}
}

void NPC::try_set_target(std::shared_ptr<Entity> _entity, bool _bWasInstigated)
{
	if(m_target.expired()) 
	{
		//*
		// If it was instigated by another entity, give this NPC a full attacking delay.
		//*
		if (_bWasInstigated)
		{
			tickCounter = get_attack_speed();
		}

		m_target = _entity;
	}
}

int NPC::get_attack_range() 
{
	return attackRange;
}

void NPC::disengage() 
{
	m_target.reset();
}


