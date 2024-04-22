#include "precomp.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Network/Client/ClientWrapper.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Global/C_Globals.h"

#include "Shared/Network/Packets/Packets.hpp"

#include <enet/enet.h> 


ENetPacketHandler::ENetPacketHandler(ENetHost* _host, ENetPeer* _peer) :
	m_host(_host), peer(_peer)
{

}

ENetPacketHandler::~ENetPacketHandler()
{
}

void ENetPacketHandler::update()
{
	//---ENET EVENT HANDLING---//
	if (enet_host_service(m_host, &m_event, 0) > 0)
	{
		switch (m_event.type)
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
	PacketHandler::retrieve_packet_data<Packets::s_PacketHeader>(header, &m_event);

	std::shared_ptr<EntityHandler> entityHandler = g_globals.entityHandler.lock();

	switch (header.interpreter)
	{
		case e_PacketInterpreter::PACKET_MOVE_PLAYER: 
		{
			Packets::s_PlayerMovement playerData;
			PacketHandler::retrieve_packet_data<Packets::s_PlayerMovement>(playerData, &m_event);

			entityHandler->get_data(playerData.fromPlayerId).set_position_from_server
			(
				Utilities::ivec2(playerData.x, playerData.y)
			);

			DEVIOUS_LOG("New position received: " << playerData.x << ", " << playerData.y);
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
			PacketHandler::retrieve_packet_data<Packets::s_PlayerPosition>(player, &m_event);
		
			PlayerDetails m_details;
			m_details.m_position.x = player.x;
			m_details.m_position.y = player.y;

			entityHandler->create_player(player.fromPlayerId, m_details);
		}
		break;

		case e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER:
		{
			Packets::s_Player player;
			PacketHandler::retrieve_packet_data<Packets::s_Player>(player, &m_event);
			entityHandler->on_local_player_assigned.invoke(player.fromPlayerId);
		}
		break;

		case e_PacketInterpreter::PACKET_DISCONNECT_PLAYER:
		{
			Packets::s_Player playerData;
			PacketHandler::retrieve_packet_data<Packets::s_Player>(playerData, &m_event);
			entityHandler->remove_player(playerData.fromPlayerId);
		}
		break;

		case e_PacketInterpreter::PACKET_TIMEOUT_WARNING:
		{
			DEVIOUS_WARN("You have been idle for a while, you will get disconnected soon if you stay idle.");
		}
		break;
	}

	enet_packet_destroy(m_event.packet);
}
