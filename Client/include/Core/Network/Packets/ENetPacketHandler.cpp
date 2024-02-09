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
			DEVIOUS_LOG("Succesfully connected to server.");
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

			DEVIOUS_LOG("New position received: " << playerPos.x << ", " << playerPos.y);
		}
		break;

		case e_PacketInterpreter::PACKET_PING:
		{
			//Respond to the ping if we can to let the server know we are still here.
			Packets::s_PacketHeader packet;
			packet.interpreter = e_PacketInterpreter::PACKET_PING;
			send_packet<Packets::s_PacketHeader>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);
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

		case e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER:
		{
			Packets::s_Player player;
			PacketHandler::retrieve_packet_data<Packets::s_Player>(player, &event);
			playerHandler->on_local_player_assigned.invoke(player.playerId);
		}
		break;

		case e_PacketInterpreter::PACKET_DISCONNECT_PLAYER:
		{
			Packets::s_Player playerData;
			PacketHandler::retrieve_packet_data<Packets::s_Player>(playerData, &event);
			playerHandler->remove_player(playerData.playerId);
		}
		break;

		case e_PacketInterpreter::PACKET_TIMEOUT_WARNING:
		{
			DEVIOUS_WARN("You have been idle for a while, you will get disconnected soon if you stay idle.");
		}
		break;
	}

	enet_packet_destroy(event.packet);
}
