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
		case e_PacketInterpreter::PACKET_MOVE_ENTITY: 
		{
			Packets::s_EntityMovement entityData;
			PacketHandler::retrieve_packet_data<Packets::s_EntityMovement>(entityData, &m_event);

			if (auto entityOpt = entityHandler->get_entity(entityData.entityId); entityOpt != std::nullopt)
			{
				RefEntity entity = entityOpt.value();
				entity->move_to(Utilities::ivec2(entityData.x, entityData.y));
			}

			DEVIOUS_LOG("New position received: " << entityData.x << ", " << entityData.y);
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

		case e_PacketInterpreter::PACKET_CREATE_ENTITY:
		{
			Packets::s_EntityPosition player;
			PacketHandler::retrieve_packet_data<Packets::s_EntityPosition>(player, &m_event);

			entityHandler->create_world_entity(player.entityId, 0, Utilities::ivec2(player.x, player.y));
		}
		break;

		case e_PacketInterpreter::PACKET_ASSIGN_LOCAL_PLAYER_ENTITY:
		{
			Packets::s_CreateEntity player;
			PacketHandler::retrieve_packet_data<Packets::s_CreateEntity>(player, &m_event);
			entityHandler->on_local_player_assigned.invoke(player.entityId);
		}
		break;

		case e_PacketInterpreter::PACKET_REMOVE_ENTITY:
		{
			Packets::s_CreateEntity entityData;
			PacketHandler::retrieve_packet_data<Packets::s_CreateEntity>(entityData, &m_event);
			entityHandler->remove_world_entity(entityData.entityId);
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
