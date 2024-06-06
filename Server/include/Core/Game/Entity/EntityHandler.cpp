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
	m_entities.clear();
}

const int32_t Server::EntityHandler::get_distance(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b)
{
	const Utilities::ivec2 delta = _b->position - _a->position;
	return std::max<int32_t>(std::abs(delta.x), std::abs(delta.y));
}

void Server::EntityHandler::register_player(const uint64_t _clientId)
{
	if (m_entities.find(_clientId) != m_entities.end())
	{
		DEVIOUS_WARN("A player already exists with this handle. " << _clientId);
		return;
	}

	auto client = g_globals.connectionHandler->get_client_info((enet_uint32)_clientId);

	m_entities[_clientId]       = std::make_shared<Player>();
	m_entities[_clientId]->uuid = client->playerId;
	m_playerToClientHandles[client->playerId] = _clientId;
	DEVIOUS_EVENT("Player " << _clientId << " has logged in.");
}

void Server::EntityHandler::logout_player(const uint64_t _clientId)
{
	if (auto player = get_entity(_clientId); player.has_value())
	{
		destroy_entity(_clientId);
		m_playerToClientHandles.erase(player.value()->uuid);

		DEVIOUS_EVENT("Player: " << _clientId << " has logged out.");
		return;
	}
	else 
	{
		DEVIOUS_WARN("Couldn't logout player: " << _clientId);
	}
}

bool Server::EntityHandler::move_entity_to(const EntityUUID _entityId, const Utilities::ivec2 _target, const bool _bIsRunning)
{
	if (m_entities.find(_entityId) == m_entities.end())
	{
		DEVIOUS_WARN("No entity data was found with the handle: " << _entityId);
		return false;
	}

	const Utilities::ivec2 entityPos = m_entities[_entityId]->position;
	
	//Check if we already gave a destination that's equal to our current position.
	if (entityPos == _target)
		return true;

	const std::vector<Utilities::ivec2> path = DM::Path::AStar::find_path(entityPos, _target);

	//Check if there's a path to the destination.
	if (path.size() > 0)
	{
		Utilities::ivec2 nextPos;

		if (_bIsRunning && path.size() > 1)
		{
			nextPos = path[1];
		}
		else
		{
			nextPos = path[0];
		}

		//Move the Entity.
		m_entities[_entityId]->position = nextPos;

		//Send packet
		{
			Packets::s_EntityMovement packet;
			{
				packet.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
				packet.entityId = m_entities[_entityId]->uuid;
				packet.x = nextPos.x;
				packet.y = nextPos.y;
			}

			PacketHandler::send_packet_multicast<Packets::s_EntityMovement>(&packet, g_globals.networkHandler->get_server_host(), 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}

	return m_entities[_entityId]->position == _target;
}

bool Server::EntityHandler::move_towards_entity(const EntityUUID _entityA, const EntityUUID _entityB, const bool _BIsRunning)
{
	uint64_t _enttA, _enttB;

	//*
	// See if any of these UUID's is relevant to a client.
	//*
	{
		if (auto optHandle = transpose_player_to_client_handle(_entityA); optHandle.has_value())
		{
			_enttA = optHandle.value();
		}
		else _enttA = _entityA;

		if (auto optHandle = transpose_player_to_client_handle(_entityB); optHandle.has_value())
		{
			_enttB = optHandle.value();
		}
		else _enttB = _entityB;
	}

	auto optEntityA = g_globals.entityHandler->get_entity(_enttA);
	auto optEntityB = g_globals.entityHandler->get_entity(_enttB);

	//*
	// Check if both handles are actually valid.
	//*
	if (optEntityA.has_value() && optEntityB.has_value())
	{
		const Utilities::ivec2 startPos = optEntityA.value()->position;
		const Utilities::ivec2 entityPos = optEntityB.value()->position;

		//Calculate the nearest adjacent tile to the target entity.
		const auto get_shortest_neighbouring_tile = [&startPos, &entityPos]()
		{
			const std::array<Utilities::ivec2, 4> neighbours
			{
				Utilities::ivec2(entityPos.x,     entityPos.y - 1), //Up
				Utilities::ivec2(entityPos.x,     entityPos.y + 1), //Down
				Utilities::ivec2(entityPos.x + 1, entityPos.y),     //Right
				Utilities::ivec2(entityPos.x - 1, entityPos.y)      //Left
			};

			std::pair<int32_t, int> closest = std::make_pair<int32_t, int>(0, INT_MAX);

			for (int32_t i = 0; i < neighbours.size(); i++)
			{
				const Utilities::ivec2 node = neighbours[i];
				const int cost = abs(startPos.x - node.x) + abs(startPos.y - node.y);

				if (cost < closest.second)
				{
					closest = { i, cost };
				}
			}

			return neighbours[closest.first];
		};

		const Utilities::ivec2 target = get_shortest_neighbouring_tile();

		//Move the entity to the target adjacent tile.
		return g_globals.entityHandler->move_entity_to(_enttA, target, _BIsRunning);
	}

	return false;
}

void Server::EntityHandler::create_world_npc(uint8_t npcId, Utilities::ivec2 _pos, int32_t _respawnTimer)
{
	NPC data              = get_entity_data(npcId);
	data.uuid             = DM::Utils::UUID::generate();
	data.position         = _pos;
	data.respawnLocation  = _pos;
	data.m_respawnTimer   = _respawnTimer;
	data.m_bHideEntity    = false;

	m_entities[data.uuid] = std::make_shared<NPC>(data);
	
	m_npcHandles.push_back(data.uuid);

	//*------------------------------------------------
	// Notice to clients that a new entity has spawned.
	//*
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

const std::vector<std::shared_ptr<NPC>> Server::EntityHandler::get_world_npcs()
{
	std::vector<std::shared_ptr<NPC>> npcs;

	for(const EntityUUID handle : m_npcHandles) 
	{
		if(m_entities.find(handle) != m_entities.end()) 
		{
			npcs.push_back
			(
				std::dynamic_pointer_cast<NPC>(m_entities.at(handle))
			);
		}
	}

	return npcs;
}

const std::vector<std::shared_ptr<Entity>> Server::EntityHandler::get_all_entities()
{
	std::vector<std::shared_ptr<Entity>> entities;

	for(auto&[uuid, entity] : m_entities) 
	{
		entities.push_back(entity);
	}

	return entities;
}

std::optional<std::shared_ptr<Entity>> Server::EntityHandler::get_entity(const DM::Utils::UUID& _id)
{
	//If the handle turns out to be a playerhandle, transpose it to the client handle and find the appropriate player.
	if(m_playerToClientHandles.find(_id) != m_playerToClientHandles.end())
	{
		uint64_t clientHandle = m_playerToClientHandles[_id];
		return m_entities[clientHandle];
	}

	//Try finding a NPC with this indentifier.
	{
		if(m_entities.find(_id) != m_entities.end()) 
		{
			return m_entities[_id];
		}
	}

	return std::nullopt;
}

std::optional<std::shared_ptr<Player>> Server::EntityHandler::get_player_by_name(const std::string& _name)
{
	for(auto&[playerId, clientId] : m_playerToClientHandles) 
	{
		auto optEntity = get_entity(clientId);

		if(optEntity.has_value()) 
		{
			std::shared_ptr<Player> player = std::static_pointer_cast<Player>(optEntity.value());

			if(player->get_name() == _name)
			{
				return player;
			}
		}
	}

	return std::nullopt;
}

const std::optional<uint64_t> Server::EntityHandler::transpose_player_to_client_handle(DM::Utils::UUID _uuid) const
{
	if(m_playerToClientHandles.find(_uuid) != m_playerToClientHandles.end()) 
	{
		return m_playerToClientHandles.at(_uuid);
	}

	return std::nullopt;
}

void Server::EntityHandler::destroy_entity(DM::Utils::UUID _uuid)
{
	m_toRemove.push_back(_uuid);
}

void Server::EntityHandler::tick()
{
	for (DM::Utils::UUID enttId : m_toRemove)
	{
		if (m_entities.find(enttId) != m_entities.end())
		{
			Packets::s_CreateEntity playerData;
			playerData.interpreter = e_PacketInterpreter::PACKET_REMOVE_ENTITY;
			playerData.entityId    = m_entities[enttId]->uuid;

			PacketHandler::send_packet_multicast<Packets::s_CreateEntity>
			(
				&playerData,
				g_globals.networkHandler->get_server_host(),
				0,
				ENET_PACKET_FLAG_RELIABLE
			);

			m_entities.erase(enttId);
		}
	}

	m_toRemove.clear();

	for(const auto&[uuid, entity] : m_entities) 
	{
		entity->update();
	}
}
