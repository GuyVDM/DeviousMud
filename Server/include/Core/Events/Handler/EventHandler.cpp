#include "precomp.h"

#include "Core/Events/Handler/EventHandler.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Navigation/AStar.hpp"

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

		case e_PacketInterpreter::PACKET_FOLLOW_ENTITY:
		{
			Packets::s_EntityFollow packet;
			PacketHandler::retrieve_packet_data<Packets::s_EntityFollow>(packet, _event);

			eventQuery->queue_packet
			(
				std::move(std::make_unique<Packets::s_EntityFollow>(packet))
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
		case e_PacketInterpreter::PACKET_FOLLOW_ENTITY:
			{
				auto packetFollow = transform_packet<Packets::s_EntityFollow>(std::move(packet));

				const Utilities::ivec2 startPos  = g_globals.entityHandler->get_player_position(_client->playerId);
				const Utilities::ivec2 entityPos = g_globals.entityHandler->get_player_position(packetFollow->entityId);

				const auto get_shortest_neighbouring_tile = [&startPos, &entityPos]()
				{
					const std::array<Utilities::ivec2, 4> neighbours
					{
						Utilities::ivec2(entityPos.x,     entityPos.y - 1), //Up
						Utilities::ivec2(entityPos.x,     entityPos.y + 1), //Down
						Utilities::ivec2(entityPos.x + 1, entityPos.y), //Right
						Utilities::ivec2(entityPos.x - 1, entityPos.y) //Left
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

				if (startPos != target)
				{
					// Queue movement packet with the target being next to the target entity.
					{
						Packets::s_EntityMovement packet;
						packet.action = e_Action::SOFT_ACTION;
						packet.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
						packet.entityId = _client->playerId;
						packet.x = target.x;
						packet.y = target.y;
						packet.isRunning = true;

						_client->packetquery->queue_packet
						(
							std::move(std::make_unique<Packets::s_EntityMovement>(packet))
						);

					}
				}

				//Queue recursive following packet.
				{
					Packets::s_EntityFollow packet;
					packet.action = e_Action::SOFT_ACTION;
					packet.interpreter = e_PacketInterpreter::PACKET_FOLLOW_ENTITY;
					packet.entityId = packetFollow->entityId;

					_client->packetquery->queue_packet
					(
						std::move(std::make_unique<Packets::s_EntityFollow>(packet))
					);
				}
			}
			break;

		case e_PacketInterpreter::PACKET_MOVE_ENTITY:
			{
				const EntityUUID playerId = _client->playerId;

				auto pMovement = transform_packet<Packets::s_EntityMovement>(std::move(packet));
				bool reachedDest = g_globals.entityHandler->move_player_towards(playerId, ivec2(pMovement->x, pMovement->y), pMovement->isRunning);

				// Send updated player position back to the clients.
				// TODO: Make the getters an optional since it can cause unintended behaviour since it might return a adress of something that's invalid.
				{
					const ivec2 playerPos = g_globals.entityHandler->get_player_position(playerId);

					Packets::s_EntityMovement packet;
					packet.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
					packet.entityId = playerId;
					packet.x = playerPos.x;
					packet.y = playerPos.y;

					PacketHandler::send_packet_multicast<Packets::s_EntityMovement>(&packet, _host, 0, 0);
				}

				//Recursively calls the packet until the player either cancels it or completes the action.
				if (!reachedDest)
				{
					//Retrieve the current position after having moved the player.
					Packets::s_EntityMovement movementPacket;
					movementPacket.action      = e_Action::SOFT_ACTION;
					movementPacket.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
					movementPacket.entityId = _client->playerId;

					movementPacket.x = pMovement->x;
					movementPacket.y = pMovement->y;
					movementPacket.isRunning = pMovement->isRunning;

					_client->packetquery->queue_packet
					(
						std::make_unique<Packets::s_EntityMovement>(movementPacket)
					);
				}
			}
			break;
		}
	}
}
