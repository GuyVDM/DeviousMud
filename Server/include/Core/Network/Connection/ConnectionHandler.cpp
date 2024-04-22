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

	for (auto& pair : client_info)
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
			PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->m_peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
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
			if((ticks % PING_TICK_INTERVAL == 0 && ticks > 0) || !clientInfo->m_peer) 
			{
				clientInfo->bAwaitingPing = true;

				Packets::s_PacketHeader packet;
				packet.interpreter = e_PacketInterpreter::PACKET_PING;
				PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->m_peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
			}
		}
		else
		{
			clientInfo->ticksSinceLastResponse++;

			DEVIOUS_LOG("Client " << clientInfo->clientId << " awaiting disconnection: ticks passed: " << clientInfo->ticksSinceLastResponse);

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

	if (client_info.find(clientId) != client_info.end())
	{
		DEVIOUS_ERR("Can't register the same client twice, " << clientId << " is already registered.");
		return;
	}

	std::shared_ptr<Server::EntityHandler> pHandler = g_globals.m_entityHandler;

	auto newClient = std::make_shared<ClientInfo>();
	newClient->m_peer					  = _peer;
	newClient->clientId			      = clientId;
	newClient->fromPlayerId			  = DM::Utils::UUID::generate();
	newClient->bAwaitingPing	      = false;
	newClient->idleticks			  = 0;
	newClient->ticksSinceLastResponse = 0;
	newClient->packetquery			  = new EventQuery();

	client_info[clientId] = newClient;     //Generate Client info
	client_handles.push_back(clientId); //Register client handle

	//Register our player in the player handler.
	pHandler->register_player(newClient->fromPlayerId);

	ENetHost* server = g_globals.networkHandler->get_server_host();

	//Create player data associated with the client.
	{
		Packets::s_PlayerPosition packet;
		packet.interpreter = e_PacketInterpreter::PACKET_CREATE_PLAYER;
		packet.fromPlayerId = newClient->fromPlayerId;
		packet.x = 0;
		packet.y = 0;

		//Sign to all clients that are already connected that a new player has joined.
		PacketHandler::send_packet_multicast<Packets::s_PlayerPosition>(&packet, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}

	//Send all existing playerdata over to the registered player.
	for(enet_uint32 handle : client_handles) 
	{
		if (clientId == handle) //If it's our own client, skip.
			continue;
		{
			const RefClientInfo clientInfo = client_info[handle];
			const Utilities::ivec2 playerPos = pHandler->get_player_position(clientInfo->fromPlayerId);

			Packets::s_PlayerPosition packet;
			packet.interpreter = e_PacketInterpreter::PACKET_CREATE_PLAYER;
			packet.fromPlayerId = clientInfo->fromPlayerId;
			packet.x = playerPos.x;
			packet.y = playerPos.y;
			PacketHandler::send_packet<Packets::s_PlayerPosition>(&packet, newClient->m_peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}

	//Send a packet to the client so they can indentify their local player.
	{
		Packets::s_Player player;
		player.interpreter = e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER;
		player.fromPlayerId = newClient->fromPlayerId;
		PacketHandler::send_packet<Packets::s_Player>(&player, newClient->m_peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}
}

void Server::ConnectionHandler::disconnect_client(const enet_uint32& _clienthandle)
{
	if (client_info.find(_clienthandle) == client_info.end())
	{
		DEVIOUS_WARN("Couldn't delete client data, handle (" << _clienthandle << ") doesn't exist.");
		return;
	}

	enet_peer_disconnect_now(client_info[_clienthandle]->m_peer, NULL);

	//Logout the player
	const uint64_t fromPlayerId = client_info[_clienthandle]->fromPlayerId;
	g_globals.m_entityHandler->logout_player(fromPlayerId);

	//Multicast to all other players that a player has left.
	Packets::s_Player playerData;
	playerData.interpreter = e_PacketInterpreter::PACKET_DISCONNECT_PLAYER;
	playerData.fromPlayerId = client_info[_clienthandle]->fromPlayerId;
	
	PacketHandler::send_packet_multicast<Packets::s_Player>
	(
		&playerData, 
		g_globals.networkHandler->get_server_host(), 
		0,
		ENET_PACKET_FLAG_RELIABLE
	);

	//Find and remove the clienthandle of the client that we're disconnecting.
	auto it = std::find_if(client_handles.begin(), client_handles.end(), [&_clienthandle](const enet_uint32& _handle)
		{
			return _clienthandle == _handle;
		});

	if (it != client_handles.end())
	{
		client_handles.erase(it);
	}

	//Remove client entry
	client_info.erase(_clienthandle);
}

RefClientInfo Server::ConnectionHandler::get_client_info(const enet_uint32& _clienthandle)
{
	return client_info.find(_clienthandle) == client_info.end() ? nullptr : client_info[_clienthandle];
}

const std::vector<enet_uint32>& Server::ConnectionHandler::get_client_handles() const
{
	return client_handles;
}


