#include "precomp.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Player/PlayerHandler.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Config/Config.h"

#include "Core/Globals/S_Globals.h"

#include <enet/enet.h>

void Server::ConnectionHandler::update_idle_timers()
{
	ENetHost* server = g_globals.networkHandler->get_server_host();

	std::vector<enet_uint32> toDisconnect;

	for (auto& pair : m_clientInfo)
	{
		RefClientInfo clientInfo = pair.second;
		uint32_t ticks = clientInfo->idleticks++;

		//Either send the player a warning with the amount of ticks he's been inactive or completely disconnect the player.
		//
		//
		if(ticks == TICKS_TILL_TIMEOUT_WARNING) 
		{
			Packets::s_PacketHeader packet;
			packet.interpreter = e_PacketInterpreter::PACKET_TIMEOUT_WARNING;
			PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
		}
		else if(ticks >= TICKS_TILL_TIMEOUT) 
		{
			toDisconnect.push_back(clientInfo->clientId);
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
				toDisconnect.push_back(clientInfo->clientId);
				continue;
			}
		}
	}

	//Disconnect all timed out client handles.
	if(toDisconnect.size() > 0) 
	{
		for (enet_uint32 clientHandle : toDisconnect) 
		{
			disconnect_client(clientHandle);
		}
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

	std::shared_ptr<Server::EntityHandler> pHandler = g_globals.entityHandler;

	auto newClient = std::make_shared<ClientInfo>();
	newClient->peer					  = _peer;
	newClient->clientId			      = clientId;
	newClient->fromEntityId			  = DM::Utils::UUID::generate();
	newClient->bAwaitingPing	      = false;
	newClient->idleticks			  = 0;
	newClient->ticksSinceLastResponse = 0;
	newClient->packetquery			  = new EventQuery();

	m_clientInfo[clientId] = newClient;     //Generate Client info
	m_clientHandles.push_back(clientId); //Register client handle

	//Register our player in the player handler.
	pHandler->register_player(newClient->fromEntityId);

	ENetHost* m_server = g_globals.networkHandler->get_server_host();

	//Create player data associated with the client.
	{
		Packets::s_EntityPosition packet;
		packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
		packet.fromEntityId = newClient->fromEntityId;
		packet.x = 0;
		packet.y = 0;

		//Sign to all clients that are already connected that a new player has joined.
		PacketHandler::send_packet_multicast<Packets::s_EntityPosition>(&packet, m_server, 0, ENET_PACKET_FLAG_RELIABLE);
	}

	//Send all existing playerdata over to the registered player.
	for(enet_uint32 handle : m_clientHandles) 
	{
		if (clientId == handle) //If it's our own client, skip.
			continue;
		{
			const RefClientInfo clientInfo = m_clientInfo[handle];
			const Utilities::ivec2 playerPos = pHandler->get_player_position(clientInfo->fromEntityId);

			Packets::s_EntityPosition packet;
			packet.interpreter = e_PacketInterpreter::PACKET_CREATE_ENTITY;
			packet.fromEntityId = clientInfo->fromEntityId;
			packet.x = playerPos.x;
			packet.y = playerPos.y;
			PacketHandler::send_packet<Packets::s_EntityPosition>(&packet, newClient->peer, m_server, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}

	//Send a packet to the client so they can indentify their local player.
	{
		Packets::s_Entity player;
		player.interpreter = e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER_ENTITY;
		player.fromEntityId = newClient->fromEntityId;
		PacketHandler::send_packet<Packets::s_Entity>(&player, newClient->peer, m_server, 0, ENET_PACKET_FLAG_RELIABLE);
	}
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
	const uint64_t fromEntityId = m_clientInfo[_clienthandle]->fromEntityId;
	g_globals.entityHandler->logout_player(fromEntityId);

	//Multicast to all other players that a player has left.
	Packets::s_Entity playerData;
	playerData.interpreter = e_PacketInterpreter::PACKET_REMOVE_ENTITY;
	playerData.fromEntityId = m_clientInfo[_clienthandle]->fromEntityId;
	
	PacketHandler::send_packet_multicast<Packets::s_Entity>
	(
		&playerData, 
		g_globals.networkHandler->get_server_host(), 
		0,
		ENET_PACKET_FLAG_RELIABLE
	);

	//Find and remove the clienthandle of the client that we're disconnecting.
	auto it = std::find_if(m_clientHandles.begin(), m_clientHandles.end(), [&_clienthandle](const enet_uint32& _handle)
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


