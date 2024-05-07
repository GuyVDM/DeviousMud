#include "precomp.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Globals/S_Globals.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Utilities/Globals.hpp"

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Navigation/AStar.hpp"

#include <cmath>

using Path = std::vector<Utilities::ivec2>;

Server::EntityHandler::~EntityHandler() 
{
	m_players.clear();
	m_entities.clear();
}

void Server::EntityHandler::register_player(const EntityUUID _playerId)
{
	if (m_entities.find(_playerId) != m_entities.end())
	{
		DEVIOUS_WARN("A player already exists with this handle. " << _playerId);
		return;
	}

	m_players[_playerId] = std::make_shared<Player>();

	DEVIOUS_EVENT("Player " << _playerId << " has logged in.");
}

void Server::EntityHandler::logout_player(const EntityUUID _playerId)
{
	if (m_entities.find(_playerId) == m_entities.end())
	{
		DEVIOUS_WARN("Couldn't logout player: " << _playerId);
		return;
	}

	DEVIOUS_EVENT("Player: " << _playerId << " has logged out.");
	m_entities.erase(_playerId);
}

//void Server::EntityHandler::add_experience(const EntityUUID _playerId, const DM::SKILLS::e_skills _skilltype, const uint32_t _experience)
//{
//	if(m_entities.find(_playerId) == m_entities.end())
//	{
//   		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
//		return;
//	}
//
//	//Get player reference.
//	std::shared_ptr<PlayerData> player = m_players[_playerId];
//
//	//Grab skill
//	DM::SKILLS::Skill* Skill = &player->skills[_skilltype];
//
//	//Calculate total exp
//	int32_t exp = Skill->experience + _experience;
//
//	//Prevent exp from going over the experience cap.
//	exp = min(exp, DM::GLOBALS::GAMECONFIG::EXPERIENCE_CAP);
//
//	//Apply the experience
//	Skill->experience = exp;
//
//	//Check if there's a level up.
//	if(Skill->level < DM::GLOBALS::GAMECONFIG::get_level(exp))
//	{
//		Skill->level++;
//
//		if(Skill->levelboosted < Skill->level) 
//		{
//			Skill->levelboosted++;
//		}
//
//		DEVIOUS_EVENT("Player " << _playerId << "has leveled up a skill.");
//	}
//}

bool Server::EntityHandler::move_player_towards(const EntityUUID _playerId, const Utilities::ivec2 _target, const bool& _isRunning)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId);
		return false;
	}

	const Utilities::ivec2 playerPos = m_players[_playerId]->position;
	
	//Check if we already gave a destination that's equal to our current position.
	if (playerPos == _target) 
		return true;

	const std::vector<Utilities::ivec2> path = DM::Path::AStar::find_path(playerPos, _target);

	//Check if there's a path to the destination.
	if (path.size() > 0)
	{
		Utilities::ivec2 nextPos;

		if (_isRunning && path.size() > 1)
		{
			nextPos = path[1];
		}
		else
		{
			nextPos = path[0];
		}

		//Move the player one position.
		set_player_position(_playerId, nextPos);
	}

	return m_players[_playerId]->position == _target;
}

void Server::EntityHandler::set_player_position(const EntityUUID _playerId, const Utilities::ivec2 _target)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
		return;
	}

	m_players[_playerId]->position = _target;
}

const Utilities::ivec2 Server::EntityHandler::get_player_position(const EntityUUID _playerId)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_ERR("No player data was found with the handle given");
		return { Utilities::ivec2(0) };
	}

	return m_players[_playerId]->position;
}

void Server::EntityHandler::create_entity(uint8_t npcId, Utilities::ivec2 _pos)
{
	NPC data = get_entity_data(npcId);
	data.uuid = DM::Utils::UUID::generate();
	data.position = _pos;
	data.startingPosition = _pos;

	m_entities[data.uuid] = std::make_shared<NPC>(data);

	//Notice to clients that a new entity has spawned.
	{
		Packets::s_CreateEntity packet;
		packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
		packet.entityId = data.uuid;
		packet.npcId = npcId;
		packet.posX = _pos.x;
		packet.posY = _pos.y;

		PacketHandler::send_packet_multicast<Packets::s_CreateEntity>
		(
			&packet,
			g_globals.networkHandler->get_server_host(),
			0,
			ENET_PACKET_FLAG_RELIABLE
		);
	}
}

bool Server::EntityHandler::move_entity_towards(NPC& _npc, Utilities::ivec2 _target)
{
	const Utilities::ivec2 entityPos = _npc.position;

	//Check if we already gave a destination that's equal to our current position.
	if (entityPos == _target)
		return true;

	const std::vector<Utilities::ivec2> path = DM::Path::AStar::find_path(entityPos, _target);

	//Check if there's a path to the destination.
	if (path.size() > 0)
	{
		Utilities::ivec2 nextPos;
		{
			nextPos = path[0];
		}

		//Move the player one position.
		_npc.position = nextPos;
	}

	Packets::s_EntityMovement entity;
	{
		entity.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
		entity.entityId = _npc.uuid;
		entity.x = _npc.position.x;
		entity.y = _npc.position.y;
		entity.isRunning = false;
	}

	PacketHandler::send_packet_multicast<Packets::s_EntityMovement>
	(
		&entity,
		g_globals.networkHandler->get_server_host(), 
		0,
		ENET_PACKET_FLAG_RELIABLE
	);

	return _npc.position == _target;
}

const std::unordered_map<EntityUUID, std::shared_ptr<NPC>>& Server::EntityHandler::get_world_entities()
{
	return m_entities;
}

void Server::EntityHandler::entities_tick()
{
	for(const auto& entity : m_entities) 
	{
		entity.second->tick();
	}
}
