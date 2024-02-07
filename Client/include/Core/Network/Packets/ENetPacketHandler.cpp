#include "precomp.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Network/Client/ClientWrapper.h"

#include "Core/Player/PlayerHandler.h"

#include "Core/Global/C_Globals.h"

#include "Shared/Network/Packets/Packets.hpp"

#include <enet/enet.h> 


ENetPacketHandler::ENetPacketHandler(ENetHost* _host, ENetPeer* _peer) :
	host(_host), peer(_peer)
{

}

ENetPacketHandler::~ENetPacketHandler()
{
}

void ENetPacketHandler::update()
{
	//---ENET EVENT HANDLING---//
	if (enet_host_service(host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
		{
			//interpret & handle incoming events.
			process_packet();
		}
		break;

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			on_disconnect.invoke();
		}	
		break;
		}
	}
}

void ENetPacketHandler::process_packet()
{
	//Retrieve the interpreter from the enetevent reference to see
	//what event type we have received.
	Packets::s_PacketHeader header;
	PacketHandler::retrieve_packet_data<Packets::s_PacketHeader>(header, &event);

	std::shared_ptr<PlayerHandler> playerHandler = g_globals.playerHandler.lock();

	switch (header.interpreter)
	{
		case e_PacketInterpreter::PACKET_MOVE_PLAYER: 
		{
			Packets::s_PlayerMovement playerData;
			PacketHandler::retrieve_packet_data<Packets::s_PlayerMovement>(playerData, &event);

			Utilities::ivec2& playerPos = playerHandler->get_details(playerData.playerId).position;
			playerPos.x = playerData.x;
			playerPos.y = playerData.y;

			std::cout << "New position received: " << playerPos.x << ", " << playerPos.y << std::endl;
		}
		break;

		case e_PacketInterpreter::PACKET_CREATE_PLAYER:
		{
			Packets::s_PlayerPosition player;
			PacketHandler::retrieve_packet_data<Packets::s_PlayerPosition>(player, &event);
		
			PlayerDetails details;
			details.position.x = player.x;
			details.position.y = player.y;

			playerHandler->create_player(player.playerId, details);
		}
		break;

		case e_PacketInterpreter::PACKET_DISCONNECT_PLAYER:
		{
			Packets::s_Player playerData;
			PacketHandler::retrieve_packet_data<Packets::s_Player>(playerData, &event);

			//Remove player.
			playerHandler->remove_player(playerData.playerId);
		}
		break;

		case e_PacketInterpreter::PACKET_TIMEOUT_WARNING:
		{
			std::cout << "[SERVER WARNING] You have been idle for a while, you will get disconnected soon if you stay idle." << std::endl;
		}
		break;
	}

	enet_packet_destroy(event.packet);
}
