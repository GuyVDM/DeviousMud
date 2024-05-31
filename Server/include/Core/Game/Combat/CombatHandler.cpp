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
	if (_b->skills[DM::SKILLS::e_skills::HITPOINTS].levelboosted <= 0)
		return false;

	DEVIOUS_EVENT("Entities are engaging! Player vs entity.");

	//Try transpose playerUUID to indentify which clienthandle owns it.
	if (std::optional<uint64_t> optHandle = g_globals.entityHandler->transpose_player_to_client_handle(_a->uuid); optHandle.has_value())
	{
		RefClientInfo client = g_globals.connectionHandler->get_client_info
		(
			static_cast<enet_uint32>(optHandle.value())
		);

		//Queue looping combat packet
		{
			Packets::s_ActionPacket packet;
			packet.interpreter = e_PacketInterpreter::PACKET_ENGAGE_ENTITY;
			packet.action      = e_Action::SOFT_ACTION;
			packet.entityId    = _b->uuid;

			client->packetquery->queue_packet
			(
				std::make_unique<Packets::s_ActionPacket>(packet)
			);
		}

		//TODO: In the future when we introduce items, we want to make the attack range dependent on attack style and weapons.
		//      For NPC's this would be predefined based on the attack they do.
		if (Server::EntityHandler::get_distance(_a, _b) > 1)
		{
			uint64_t playerHandle = g_globals.entityHandler->transpose_player_to_client_handle(_a->uuid).value();
			g_globals.entityHandler->move_entity_towards(playerHandle, _b->position);

			return false;
		}

		if (_a->tickCounter <= 0)
		{
			const int32_t tempAttackDelayTicks = 1;

			_a->tickCounter = tempAttackDelayTicks;

			hit(_a, _b);
		}
		else
		{
			_a->tickCounter--;
			return false;
		}

		//*
		//	If the Target Entity still has health left.
		//*
		if (_b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].levelboosted >= 0)
		{
			Packets::s_ActionPacket packet;
			{
				packet.interpreter = e_PacketInterpreter::PACKET_ENGAGE_ENTITY;
				packet.action = e_Action::SOFT_ACTION;
				packet.entityId = _b->uuid;

				client->packetquery->queue_packet
				(
					std::make_unique<Packets::s_ActionPacket>(packet)
				);
			}
		}

		return true;
	}

	return false;
}

const bool CombatHandler::engage(std::shared_ptr<NPC> _a, std::shared_ptr<Entity> _b)
{
	if (_b->skills[DM::SKILLS::e_skills::HITPOINTS].levelboosted <= 0)
		return false;

	if (Server::EntityHandler::get_distance(_a, _b) <= _a->get_attack_range())
	{
		if (_a->tickCounter <= 0)
		{
			_a->tickCounter = _a->tickCounter++;

			hit(_a, _b);

			if (_b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].levelboosted >= 0)
			{
				DEVIOUS_EVENT("Died.");
			}

			return true;
		}
		else
		{
			_a->tickCounter--;
			return false;
		}

	}
	
	g_globals.entityHandler->move_entity_towards(_a->uuid, _b->position);
	return false;

}

void CombatHandler::hit(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b)
{
	//TODO: remove this and calculate this dynamically.
	const int32_t maxHit = 1;

	int32_t* hp = &_b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].levelboosted;
	
	*hp = CLAMP(*hp - maxHit, 0, INT32_MAX);

	DEVIOUS_LOG("Entity now currently has: " << *hp << " hitpoints remaining.");

	//Send hitData over.
	{
		Packets::s_EntityHit packet;
		packet.interpreter  = e_PacketInterpreter::PACKET_ENTITY_HIT;
		packet.action       = e_Action::SOFT_ACTION;
		packet.fromEntityId = _a->uuid;
		packet.toEntityId   = _b->uuid;
		packet.hitAmount    = maxHit;

		PacketHandler::send_packet_multicast<Packets::s_EntityHit>
			(
				&packet,
				g_globals.networkHandler->get_server_host(),
				0,
				ENET_PACKET_FLAG_RELIABLE
			);
	}

	//Update entity skill clientsided.
	{
		Packets::s_UpdateSkill packet;
		packet.interpreter = e_PacketInterpreter::PACKET_ENTITY_SKILL_UPDATE;
		packet.action = e_Action::SOFT_ACTION;
		packet.entityId = _b->uuid;
		packet.skillType = (uint8_t)DM::SKILLS::e_skills::HITPOINTS;
		packet.level = _b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].level;
		packet.levelBoosted = _b->skills.get_map()[DM::SKILLS::e_skills::HITPOINTS].levelboosted;

		PacketHandler::send_packet_multicast<Packets::s_UpdateSkill>
			(
				&packet,
				g_globals.networkHandler->get_server_host(),
				0,
				ENET_PACKET_FLAG_RELIABLE
			);
	}
}
