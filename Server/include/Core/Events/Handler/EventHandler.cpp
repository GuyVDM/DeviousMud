#include "precomp.h"
#include "EventHandler.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Core/Globals/S_Globals.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Player/PlayerHandler.h"

using ivec2 = Utilities::ivec2;

void Server::EventHandler::queue_incoming_event(ENetEvent* _event, RefClientInfo& _clientInfo)
{
	EventQuery* eventQuery = _clientInfo->packetquery;

	Packets::s_PacketHeader packetHeader;
	PacketHandler::retrieve_packet_data<Packets::s_PacketHeader>(packetHeader, _event);

	switch (packetHeader.interpreter)
	{
		//TODO: Move these rpcs to a specific handler for immidiate unrelated to game events, e.g logout, ping
		case e_PacketInterpreter::PACKET_PING:
		{
			_clientInfo->bAwaitingPing = false;
			_clientInfo->ticksSinceLastResponse = 0;
		}
		return;

		case e_PacketInterpreter::PACKET_REMOVE_ENTITY:
		{
			std::shared_ptr<Server::ConnectionHandler> cHandler = g_globals.connectionHandler;
			cHandler->disconnect_client(_clientInfo->clientId);
		}
		return;

		case e_PacketInterpreter::PACKET_MOVE_ENTITY:
		{
			Packets::s_EntityMovement m_position;
			PacketHandler::retrieve_packet_data<Packets::s_EntityMovement>(m_position, _event);

			eventQuery->queue_packet
			(
				std::move(std::make_unique<Packets::s_EntityMovement>(m_position))
			);
		}
		break;
	}

	DEVIOUS_EVENT("Received packet from client handle: " << _clientInfo->peer->connectID << " Event Id: " << static_cast<unsigned>(packetHeader.interpreter));
}

void Server::EventHandler::handle_queud_events(ENetHost* _host)
{
	const std::shared_ptr<ConnectionHandler> connectionHandler = g_globals.connectionHandler;

	//Go through all connected clients.
	for(const enet_uint32& clientHandle : connectionHandler->get_client_handles()) 
	{
		RefClientInfo clientInfo = connectionHandler->get_client_info(clientHandle);

		//If the client has any tickets queud
		if (!clientInfo->packetquery->contains_packets())
			continue;

		handle_client_specific_packets(clientInfo, _host);

		//Reset Inactivity time
		clientInfo->refresh();
	}
}

void Server::EventHandler::handle_client_specific_packets(RefClientInfo& _client, ENetHost* _host)
{
	EventQuery eventQuery; 
	_client->packetquery->move(&eventQuery);

	while(eventQuery.contains_packets()) 
	{
		std::unique_ptr<Packets::s_PacketHeader> packet = eventQuery.retrieve_next();

		switch(packet->interpreter) 
		{
			case PACKET_MOVE_ENTITY:
			{
				const PlayerUUID fromEntityId = _client->fromEntityId;

				auto pMovement = transform_packet<Packets::s_EntityMovement>(std::move(packet));
				bool reachedDest = g_globals.entityHandler->move_player_towards(fromEntityId, ivec2(pMovement->x, pMovement->y), pMovement->isRunning);

				// Send updated player position back to the clients.
				// TODO: Make the getters an optional since it can cause unintended behaviour since it might return a adress of something that's invalid.
				{
					ivec2 playerPos = g_globals.entityHandler->get_player_position(fromEntityId);

					Packets::s_EntityMovement packet;
					packet.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
					packet.fromEntityId = fromEntityId;
					packet.x = playerPos.x;
					packet.y = playerPos.y;

					PacketHandler::send_packet_multicast<Packets::s_EntityMovement>(&packet, _host, 0, 0);
				}

				//Recursively calls the packet until the player either cancels it or completes the action.
				if (!reachedDest)
				{
					//Retrieve the current position after having moved the player.
					Packets::s_EntityMovement m_position;
					m_position.fromEntityId = _client->fromEntityId;
					m_position.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
					m_position.x = pMovement->x;
					m_position.y = pMovement->y;
					m_position.isRunning = pMovement->isRunning;

					_client->packetquery->queue_packet
					(
						std::make_unique<Packets::s_EntityMovement>(m_position)
					);
				}
			}
			break;
		}
	}
}
