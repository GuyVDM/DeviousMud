#include "precomp.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Config/Config.h"

#include "Core/Globals/S_Globals.h"

#include <enet/enet.h>

void Server::ConnectionHandler::update_idle_timers()
{
	ENetHost* server = g_globals.networkHandler->get_server_host();

	for (auto& pair : m_clientInfo)
	{
		RefClientInfo clientInfo = pair.second;
		uint32_t ticks = clientInfo->idleticks++;

		//Either send the player a warning with the amount of ticks he's been inactive 
		//or completely disconnect the player.
		if(ticks == TICKS_TILL_TIMEOUT_WARNING) 
		{
			Packets::s_PacketHeader packet;
			packet.interpreter = e_PacketInterpreter::PACKET_TIMEOUT_WARNING;
			PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
		}
		else if(ticks >= TICKS_TILL_TIMEOUT) 
		{
			m_pendingDisconnects.push_back((enet_uint32)clientInfo->clientId);
			DEVIOUS_LOG("Registered client: " << clientInfo->clientId << " for disconnect.");
			continue;
		}

		//Checks whether the client needs to have a ping sent out to check if the client is still in a position to get reached.
		//
		//
		if(!clientInfo->bAwaitingPing) 
		{
			if((ticks % PING_TICK_INTERVAL == 0 && ticks > 0) || !clientInfo->peer) 
			{
				clientInfo->bAwaitingPing = true;

				Packets::s_PacketHeader packet;
				packet.interpreter = e_PacketInterpreter::PACKET_PING;
				PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
			}
		}
		else
		{
			clientInfo->ticksSinceLastResponse++;

			if (clientInfo->ticksSinceLastResponse >= MAX_TICK_INTERVAL_NO_RESPONSE)
			{
				DEVIOUS_LOG("Couldnt retrieve any sign of connection from " << clientInfo->clientId << ". Disconnecting...");
				m_pendingDisconnects.push_back((enet_uint32)clientInfo->clientId);
				continue;
			}
		}
	}

	//Disconnect all timed out client handles.
	if(m_pendingDisconnects.size() > 0)
	{
		for (enet_uint32 clientHandle : m_pendingDisconnects)
		{
			disconnect_client(clientHandle);
		}

		m_pendingDisconnects.clear();
	}
}

void Server::ConnectionHandler::register_client(ENetPeer* _peer)
{
	const enet_uint32 clientId = _peer->connectID;

	if (m_clientInfo.find(clientId) != m_clientInfo.end())
	{
		DEVIOUS_ERR("Can't register the same client twice, " << clientId << " is already registered.");
		return;
	}

	RefClientInfo newClient = std::make_shared<ClientInfo>();
	{
		newClient->peer = _peer;
		newClient->clientId = (uint64_t)clientId;
		newClient->playerId = DM::Utils::UUID::generate();
		newClient->bAwaitingPing = false;
		newClient->idleticks = 0;
		newClient->ticksSinceLastResponse = 0;
		newClient->packetquery = new EventQuery();
	}

	//Generate Client info & Register the handle.
	{
		m_clientInfo[clientId] = newClient;
		m_clientHandles.push_back(clientId); 
	}

	std::shared_ptr<Server::EntityHandler> eHandler = g_globals.entityHandler;

	ENetHost* server = g_globals.networkHandler->get_server_host();

	//Register our player in the player handler.
	eHandler->register_player(newClient->clientId);

	//Create player data associated with the client.
	{
		Packets::s_CreateEntity packet;
		packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
		packet.entityId    = newClient->playerId;
		packet.npcId = 0;
		packet.posX  = 0;
		packet.posY  = 0;
		packet.bIsHidden = false;

		//Sign to all clients that are already connected that a new player has joined.
		PacketHandler::send_packet_multicast<Packets::s_CreateEntity>(&packet, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}

	//Send a packet to the client so they can indentify their local player.
	{
		Packets::s_CreateEntity player;
		player.interpreter = e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER_ENTITY;
		player.entityId = newClient->playerId;
		PacketHandler::send_packet<Packets::s_CreateEntity>(&player, newClient->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}

	//Set temporary name for the player.
	{
		auto player = std::static_pointer_cast<Player>
		(
			eHandler->get_entity(newClient->clientId).value()
		);

		const std::string name = "Player" ;
		player->set_name(name);
		player->whisper("Welcome to my DeviousMUD 2D Clone!");
		player->whisper("Use ::changename [name] to change your name ingame.");
	}

	//Send all existing players over to the registered player.
	for (const enet_uint32 handle : m_clientHandles)
	{
		if (clientId == handle) //If it's our own client, skip.
			continue;

		if (auto optPlayer = eHandler->get_entity((uint64_t)handle); optPlayer.has_value())
		{
			// Send creation packet
			{
				Packets::s_CreateEntity packet;
				packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
				packet.entityId = optPlayer.value()->uuid;
				packet.npcId = 0;
				packet.posX = optPlayer.value()->position.x;
				packet.posY = optPlayer.value()->position.y;
				packet.bIsHidden = optPlayer.value()->is_hidden();

				PacketHandler::send_packet<Packets::s_CreateEntity>(&packet, newClient->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
			}

			// Update the player its name.
			{
				std::shared_ptr<Player> player = std::static_pointer_cast<Player>(optPlayer.value());

				Packets::s_NameChange packet;
				packet.interpreter = e_PacketInterpreter::PACKET_CHANGE_NAME;
				packet.entityId = player->uuid;
				packet.name     = player->get_shown_name();
			}
		}
	}

	//Send all existing npcs over to the registered player.
	for (const auto& entity : eHandler->get_world_npcs())
	{
		Packets::s_CreateEntity packet;
		packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
		packet.entityId  = entity->uuid;
		packet.npcId     = entity->npcId;
		packet.posX      = entity->position.x;
		packet.posY      = entity->position.y;
		packet.bIsHidden = entity->is_hidden();

		PacketHandler::send_packet<Packets::s_CreateEntity>(&packet, newClient->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}
}

void Server::ConnectionHandler::flag_for_disconnect(const enet_uint32& _clienthandle)
{
	m_pendingDisconnects.push_back(_clienthandle);
}

void Server::ConnectionHandler::disconnect_client(const enet_uint32& _clienthandle)
{
	if (m_clientInfo.find(_clienthandle) == m_clientInfo.end())
	{
		DEVIOUS_WARN("Couldn't delete client data, handle (" << _clienthandle << ") doesn't exist.");
		return;
	}

	enet_peer_disconnect_now(m_clientInfo[_clienthandle]->peer, NULL);

	//Logout the player
	{
		g_globals.entityHandler->logout_player(m_clientInfo[_clienthandle]->clientId);
	}

	//Find and remove the clienthandle of the client that we're disconnecting.
	const auto it = std::find_if(m_clientHandles.begin(), m_clientHandles.end(), [&_clienthandle](const enet_uint32& _handle)
	{
		return _clienthandle == _handle;
	});

	if (it != m_clientHandles.end())
	{
		m_clientHandles.erase(it);
	}

	//Remove client entry
	m_clientInfo.erase(_clienthandle);
}

RefClientInfo Server::ConnectionHandler::get_client_info(const enet_uint32& _clienthandle)
{
	return m_clientInfo.find(_clienthandle) == m_clientInfo.end() ? nullptr : m_clientInfo[_clienthandle];
}

const std::vector<enet_uint32>& Server::ConnectionHandler::get_client_handles() const
{
	return m_clientHandles;
}


