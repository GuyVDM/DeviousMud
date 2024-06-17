#include "precomp.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Utilities/Math.hpp"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Combat/CombatHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Globals/S_Globals.h"

const bool CombatHandler::engage(std::shared_ptr<Player> _a, std::shared_ptr<Entity> _b)
{
	//*
	// Check if either entities are eligible to engage in combat.
	// In this case we check if either of them are already dead.
	//*
	{
		if (_a->is_dead() || _b->is_dead())
		{

			return false;
		}
	}

	//*
	// Try transpose playerUUID to indentify which clienthandle owns it.
	//*
	if (std::optional<uint64_t> optHandle = g_globals.entityHandler->transpose_player_to_client_handle(_a->uuid); optHandle.has_value())
	{
		RefClientInfo client = g_globals.connectionHandler->get_client_info
		(
			static_cast<enet_uint32>(optHandle.value())
		);

		const uint64_t playerHandle = g_globals.entityHandler->transpose_player_to_client_handle(_a->uuid).value();

		int32_t attackRange = _a->get_attack_range();

		//*----------------------------------------
		//Check if _A is in attacking distance of _B
		//*
		if (!in_range(_a, _b, attackRange))
		{
			g_globals.entityHandler->move_towards_entity(playerHandle, _b->uuid, true);
			
			//If we're still not in range even after moving, we don't want to register a hit.
			if(!in_range(_a, _b, attackRange)) 
			{
				queue_combat_packet(client, _b->uuid);
				return false;
			}
		}
		
		queue_combat_packet(client, _b->uuid);

		//*--------------------------------------------------------------------------------------------------------------------
		// TODO: In the future when we introduce items, we want to make the attack range dependent on attack style and weapons.
		// For NPC's this would be predefined based on the attack they do.
		//*      
		if (_a->tickCounter <= 0)
		{
			const int32_t tempAttackDelayTicks = 2;

			_a->tickCounter = tempAttackDelayTicks;

			hit(_a, _b);

			return true;
		}
	}

	return false;
}

const bool CombatHandler::engage(std::shared_ptr<NPC> _a, std::shared_ptr<Entity> _b)
{
	//*
	// Check if either entities are eligible to engage in combat.
	// In this case we check if either of them are already dead.
	//*
	{
		if (_a->is_dead() || _b->is_dead())
		{
			return false;
		}
	}

	int32_t attackRange = _a->get_attack_range();

	//*----------------------------------------
	//Check if _A is in attacking distance of _B
	//*
	if (!in_range(_a, _b, attackRange))
	{
		g_globals.entityHandler->move_towards_entity(_a->uuid, _b->uuid, false);

		//If we're still not in range even after moving, we don't want to register a hit.
		if (!in_range(_a, _b, attackRange))
		{
			return false;
		}
	}

	//*--------------------------------------------------------------------------------------------------------------------
	// TODO: In the future when we introduce items, we want to make the attack range dependent on attack style and weapons.
	// For NPC's this would be predefined based on the attack they do.
	//*      
	if (_a->tickCounter <= 0)
	{
		_a->tickCounter = _a->get_attack_speed();
		
		hit(_a, _b);

		return true;
	}
	
	return false;
}

void CombatHandler::hit(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b)
{
	//TODO: remove this and calculate this dynamically.
	const int32_t maxHit = 1;

	//Apply the hit
	_b->hit(_a, maxHit);

	//Try make other entity retaliate.
	_b->try_set_target(_a, true);

	//*------------------
	// Disengage if dead.
	//*
	{
		int32_t hp = _b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].levelboosted;
		if (hp <= 0)
		{
			_a->disengage();
		}
	}
}

void CombatHandler::queue_combat_packet(RefClientInfo _client, DM::Utils::UUID _targetUUID)
{
	Packets::s_ActionPacket packet;
	packet.interpreter = e_PacketInterpreter::PACKET_ENGAGE_ENTITY;
	packet.action      = e_Action::SOFT_ACTION;
	packet.entityId    = _targetUUID;

	_client->packetquery->queue_packet
	(
		std::make_unique<Packets::s_ActionPacket>(packet)
	);
}

bool CombatHandler::in_range(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b, int32_t _attackRange)
{
	bool bRequireAdjacent = false; //If true, means the attacking entity has to be adjacent of the target.

	const int32_t distance = Server::EntityHandler::get_distance(_a, _b);


	if (_attackRange == 0)
	{
		_attackRange = 1;
		bRequireAdjacent = true;
	}

	if (distance <= _attackRange && distance != 0)
	{
		if (bRequireAdjacent)
		{
			return CombatHandler::is_adjacent(_a, _b);
		}
		else
		{
			return true;
		}

	}

	return false;
}

bool CombatHandler::is_adjacent(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b)
{
	const Utilities::ivec2 pos       = _a->position;
	const Utilities::ivec2 targetPos = _b->position;

	if (pos == (targetPos + Utilities::ivec2(1,   0))) return true; //Right
	if (pos == (targetPos + Utilities::ivec2(-1,  0))) return true; //Left
	if (pos == (targetPos + Utilities::ivec2(0,   1))) return true; //Down
	if (pos == (targetPos + Utilities::ivec2(0,  -1))) return true; //Up
	
	return false;
}
