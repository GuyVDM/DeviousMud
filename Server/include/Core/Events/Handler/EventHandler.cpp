#include "precomp.h"

#include "Core/Events/Handler/EventHandler.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Game/Combat/CombatHandler.h"

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
			cHandler->disconnect_client((enet_uint32)_clientInfo->clientId);
		}
		return;

		case e_PacketInterpreter::PACKET_MOVE_ENTITY:
		{
			Packets::s_EntityMovement position;
			PacketHandler::retrieve_packet_data<Packets::s_EntityMovement>(position, _event);

			eventQuery->queue_packet
			(
				std::move(std::make_unique<Packets::s_EntityMovement>(position))
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

		case e_PacketInterpreter::PACKET_ENGAGE_ENTITY:
		{
			Packets::s_ActionPacket packet;
			PacketHandler::retrieve_packet_data<Packets::s_ActionPacket>(packet, _event);

			eventQuery->queue_packet
			(
				std::move(std::make_unique<Packets::s_ActionPacket>(packet))
			);
		}
		break;
	}

	DEVIOUS_EVENT("Received packet from client handle: " << _clientInfo->peer->connectID << " Event Id: " << static_cast<unsigned>(packetHeader.interpreter));
}

void Server::EventHandler::handle_queud_events()
{
	const std::shared_ptr<ConnectionHandler> connectionHandler = g_globals.connectionHandler;

	//Go through all connected clients.
	for(const enet_uint32& clientHandle : connectionHandler->get_client_handles()) 
	{
		RefClientInfo clientInfo = connectionHandler->get_client_info(clientHandle);

		//If the client has any tickets queud
		if (!clientInfo->packetquery->contains_packets())
			continue;

		handle_client_specific_packets(clientInfo);

		//Reset Inactivity time
		clientInfo->refresh();
	}
}

void Server::EventHandler::handle_client_specific_packets(RefClientInfo& _client)
{
	ENetHost* host = g_globals.networkHandler->get_server_host();

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

				g_globals.entityHandler->move_towards_entity(_client->clientId, packetFollow->entityId, true);

				//Queue looping following packet.
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
			auto enttPacket = transform_packet<Packets::s_EntityMovement>(std::move(packet));

			const bool bReachedDest = g_globals.entityHandler->move_entity_to
			(
				_client->clientId, 
				ivec2(enttPacket->x, enttPacket->y),
				enttPacket->isRunning
			);

			//Recursively calls the packet until the player either cancels it or completes the action.
			if (!bReachedDest)
			{
				//Retrieve the current position after having moved the player.
				Packets::s_EntityMovement movementPacket;
				{
					movementPacket.action = e_Action::SOFT_ACTION;
					movementPacket.interpreter = e_PacketInterpreter::PACKET_MOVE_ENTITY;
					movementPacket.entityId = _client->playerId;
					movementPacket.isRunning = enttPacket->isRunning;
					movementPacket.x = enttPacket->x;
					movementPacket.y = enttPacket->y;
				}

				_client->packetquery->queue_packet
				(
					std::make_unique<Packets::s_EntityMovement>(movementPacket)
				);
			}
		}
		break;

			case e_PacketInterpreter::PACKET_ENGAGE_ENTITY:
			{
				using OptEntity = std::optional<std::shared_ptr<Entity>>;
				
				const auto action = transform_packet<Packets::s_ActionPacket>
				(
					std::move(packet)
				);
				
				OptEntity player = g_globals.entityHandler->get_entity(_client->clientId);
				OptEntity entity = g_globals.entityHandler->get_entity(action->entityId);

				if (entity.has_value() && player.has_value())
				{
					CombatHandler::engage
					(
						std::dynamic_pointer_cast<Player>(player.value()), 
						entity.value()
					);
				}
			}
			break;
		}
	}
}
