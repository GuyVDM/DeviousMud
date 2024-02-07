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

			PacketHandler::send_packet<Packets::s_PacketHeader>(&packet, clientInfo->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
		}
		else if(ticks >= TICKS_TILL_TIMEOUT) 
		{
			toDisconnect.push_back(clientInfo->peer->connectID);
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
	std::shared_ptr<Server::PlayerHandler> pHandler = g_globals.playerHandler;

	const enet_uint32 clientId = _peer->connectID;

	auto newClient = std::make_shared<ClientInfo>();
	newClient->peer        = _peer;
	newClient->playerId    = DEVIOUSMUD::RANDOM::UUID::generate();
	newClient->idleticks   = 0;
	newClient->packetquery = new EventQuery();

	client_info[clientId] = newClient;     //Generate Client info
	client_handles.push_back(clientId); //Register client handle.;

	//Register our player in the player handler.
	pHandler->register_player(newClient->playerId);

	//Create player data associated with the client.
	Packets::s_PlayerPosition packet;
	packet.interpreter = e_PacketInterpreter::PACKET_CREATE_PLAYER;
	packet.playerId = newClient->playerId;
	packet.x = 0;
	packet.y = 0;

	ENetHost* server = g_globals.networkHandler->get_server_host();

	//Sign to all clients that are already connected that a new player has joined.
	PacketHandler::send_packet_multicast<Packets::s_PlayerPosition>(&packet, server, 0, ENET_PACKET_FLAG_RELIABLE);

	//Send all existing playerdata over to the registered player.
	for(enet_uint32 handle : client_handles) 
	{
		if (clientId == handle) //If it's our own client, skip.
			continue;

		const RefClientInfo clientInfo = client_info[handle];
		const Utilities::ivec2 playerPos = pHandler->get_player_position(clientInfo->playerId);

		packet.playerId = clientInfo->playerId;
		packet.x = playerPos.x;
		packet.y = playerPos.y;
		PacketHandler::send_packet<Packets::s_PlayerPosition>(&packet, newClient->peer, server, 0, ENET_PACKET_FLAG_RELIABLE);
	}
}

void Server::ConnectionHandler::disconnect_client(const enet_uint32& _clienthandle)
{
	if(client_info.find(_clienthandle) == client_info.end()) 
	{
		std::cout << "Couldn't disconnect client, handle doesn't exist" << std::endl;
		return;
	}

	enet_peer_disconnect_now(client_info[_clienthandle]->peer, NULL);

	//Logout the player
	g_globals.playerHandler->logout_player(client_info[_clienthandle]->playerId);

	Packets::s_Player playerData;
	playerData.interpreter = e_PacketInterpreter::PACKET_DISCONNECT_PLAYER;
	playerData.playerId = client_info[_clienthandle]->playerId;

	ENetHost* server = g_globals.networkHandler->get_server_host();
	PacketHandler::send_packet_multicast(&playerData, server, 0, ENET_PACKET_FLAG_RELIABLE);

	//Find and remove the clienthandle of the client that we're disconnecting.
	auto it = std::find_if(client_handles.begin(), client_handles.end(), [&_clienthandle](const enet_uint32& _handle)
		{
			return _clienthandle == _handle;
		});

	if(it != client_handles.end()) 
	{
		client_handles.erase(it);
	}

	//Remove client entry
	client_info.erase(_clienthandle);
	std::cout << "Disconnected client handle: " << _clienthandle << std::endl;
}

RefClientInfo Server::ConnectionHandler::get_client_info(const enet_uint32& _clienthandle)
{
	if(client_info.find(_clienthandle) == client_info.end()) 
	{
		return nullptr;
	}

	return client_info[_clienthandle];
}

const std::vector<enet_uint32>& Server::ConnectionHandler::get_client_handles() const
{
	return client_handles;
}
