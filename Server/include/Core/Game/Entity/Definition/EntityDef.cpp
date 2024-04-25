#include "precomp.h"

#include "Core/Game/Entity/Definition/EntityDef.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Game/Entity/EntityHandler.h"

#include <random>

/// <summary>
/// TODO: Replace these with statemachines maybe?
/// </summary>
void NPC::tick()
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

	if(m_bIsMoving) 
	{
		g_globals.entityHandler->move_entity_towards(*this, m_targetPos);

		if(m_targetPos == position) 
		{
			m_bIsMoving = false;
		}
	}
}
