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
}

void Entity::tick()
{
	/// Override in child classes.
	///
	/// 
}

void Entity::set_target(std::shared_ptr<Entity> _entity)
{
	/// Override in child classes.
	///
	///
}

/// <summary>
/// TODO: Replace these with statemachines maybe?
/// </summary>
void NPC::tick()
{
	if(m_target) 
	{
		auto optNPC    = g_globals.entityHandler->get_entity(uuid);

		CombatHandler::engage
		(
			std::dynamic_pointer_cast<NPC>(optNPC.value()),
			m_target
		);
		return;
	}

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

	if(m_bIsMoving) 
	{
		g_globals.entityHandler->move_entity_to(uuid, m_targetPos);

		if(m_targetPos == position) 
		{
			m_bIsMoving = false;
		}
	}
}

void NPC::set_target(std::shared_ptr<Entity> _entity)
{
	if(!m_target) 
	{
		m_target = _entity;
	}
}

int NPC::get_attack_range() 
{
	return attackRange;
}

int Entity::get_attack_range() 
{
	return 1;
}

