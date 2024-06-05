#include "precomp.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Network/Client/ClientWrapper.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/Chatbox/Chatbox.h"

#include "Shared/Network/Packets/Packets.hpp"

#include <enet/enet.h> 

/// We ignore the initialization warning since the ENetEvent doesn't need to get initialized.
///
/// 
#pragma warning(push)
#pragma warning(disable: 4700)

ENetPacketHandler::ENetPacketHandler(ENetHost* _host, ENetPeer* _peer) :
	m_host(_host), m_peer(_peer)
{

}

#pragma warning(pop)

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
				return;
			}

			DEVIOUS_WARN("Tried to move non existing entity. " << entityData.entityId);
		}
		break;

		case e_PacketInterpreter::PACKET_PING:
		{
			Packets::s_PacketHeader packet;
			packet.interpreter = e_PacketInterpreter::PACKET_PING;
			send_packet<Packets::s_PacketHeader>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);
		}
		break;

		case e_PacketInterpreter::PACKET_CREATE_ENTITY:
		{
			Packets::s_CreateEntity packet;
			PacketHandler::retrieve_packet_data<Packets::s_CreateEntity>(packet, &m_event);
			entityHandler->create_world_entity(packet.entityId, packet.npcId, Utilities::ivec2(packet.posX, packet.posY));
			
			RefEntity entity = entityHandler->get_entity(packet.entityId).value();
			entity->set_visibility(packet.bIsHidden);

			DEVIOUS_EVENT("Creating Entity of id: " << packet.npcId << " at coords: " << packet.posX << ", " << packet.posY << ".");
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

		case e_PacketInterpreter::PACKET_ENTITY_HIT:
		{
			Packets::s_EntityHit packet;
			PacketHandler::retrieve_packet_data<Packets::s_EntityHit>(packet, &m_event);

			if(auto instigatorEnttOpt = g_globals.entityHandler.lock()->get_entity(packet.fromEntityId); instigatorEnttOpt.has_value())
			{
				RefEntity instigatorEntt = instigatorEnttOpt.value();
				instigatorEntt->attack();
				instigatorEntt->turn_to(packet.toEntityId);
			}

			if (auto victimEnttOpt = g_globals.entityHandler.lock()->get_entity(packet.toEntityId); victimEnttOpt.has_value())
			{
				RefEntity victimEntt = victimEnttOpt.value();
				victimEntt->hit(packet.fromEntityId, packet.hitAmount);
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_SKILL_UPDATE:
		{
			Packets::s_UpdateSkill packet;
			PacketHandler::retrieve_packet_data<Packets::s_UpdateSkill>(packet, &m_event);

			if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
			{
				RefEntity entt = optEntity.value();
				entt->update_skill(packet.skillType, packet.level, packet.levelBoosted);
				return;
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_DEATH:
		{
			Packets::s_ActionPacket packet;
			PacketHandler::retrieve_packet_data<Packets::s_ActionPacket>(packet, &m_event);

			if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
			{
				RefEntity entt = optEntity.value();
				entt->die();
				return;
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_RESPAWN:
		{
			Packets::s_ActionPacket packet;
			PacketHandler::retrieve_packet_data<Packets::s_ActionPacket>(packet, &m_event);

			if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
			{
				RefEntity entt = optEntity.value();
				entt->respawn();
				return;
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_HIDE:
		{
			Packets::s_HideEntity packet;
			PacketHandler::retrieve_packet_data<Packets::s_HideEntity>(packet, &m_event);

			if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
			{
				RefEntity entt = optEntity.value();
				entt->set_visibility(packet.bShouldHide);
				return;
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_TELEPORT:
		{
			Packets::s_TeleportEntity packet;
			PacketHandler::retrieve_packet_data<Packets::s_TeleportEntity>(packet, &m_event);

			if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
			{
				RefEntity entt = optEntity.value();
				entt->teleport_to
				(
					Utilities::vec2
					(
						static_cast<float>(packet.x), 
						static_cast<float>(packet.y)
					)
				);
			}
		}
		break;

		case e_PacketInterpreter::PACKET_ENTITY_MESSAGE_WORLD:
		{
			Packets::s_Message packet;
			PacketHandler::retrieve_packet_data<Packets::s_Message>(packet, &m_event);

			if (packet.entityId != 0)
			{
				if (auto optEntity = g_globals.entityHandler.lock()->get_entity(packet.entityId); optEntity.has_value())
				{
					RefEntity entt = optEntity.value();
					entt->say(packet.message);
				}
			}

			ChatboxMessage message;
			message.name = packet.author;
			message.message = packet.message;

			Chatbox::s_on_message_received.invoke(message);
		}
		break;

	}

	enet_packet_destroy(m_event.packet);
}
