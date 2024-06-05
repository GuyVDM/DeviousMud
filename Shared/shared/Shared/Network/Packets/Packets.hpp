#pragma once
#include <fstream>
#include <vector>
#include <utility>
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"

enum class e_Action : uint8_t
{
	SOFT_ACTION   = 0x00,

	MEDIUM_ACTION = 0x01, 

	HARD_ACTION   = 0x02 
};

enum class e_PacketInterpreter : uint8_t
{
	PACKET_NONE = 0x00,

	PACKET_PING = 0x01,

	PACKET_CREATE_ENTITY = 0x02,

	PACKET_ASSIGN_LOCAL_PLAYER_ENTITY = 0x03,

	PACKET_REMOVE_ENTITY = 0x04,

	PACKET_MOVE_ENTITY = 0x05,

	PACKET_TIMEOUT_WARNING = 0x06,

	PACKET_PLAYER_PATH = 0x07,

	PACKET_FOLLOW_ENTITY = 0x08,

	PACKET_ENGAGE_ENTITY = 0x09,

	PACKET_ENTITY_HIT    = 0x0A,

	PACKET_ENTITY_SKILL_UPDATE = 0x0B,

	PACKET_ENTITY_DEATH        = 0x0C,

	PACKET_ENTITY_RESPAWN      = 0x0D,

	PACKET_ENTITY_HIDE         = 0x0E,

	PACKET_ENTITY_TELEPORT     = 0x0F,

	PACKET_ENTITY_MESSAGE_WORLD = 0x10,

	PACKET_CHANGE_NAME          = 0x11
};

namespace Packets
{
	/// <summary>
	/// All further packets should implement this as their base with each packet their first bit representing the identifier.
	/// </summary>
	struct s_PacketHeader
	{
		/// <summary>
		/// Interpreter is there to see how the remainder of the data should be interpreted.
		/// </summary>
		e_PacketInterpreter interpreter = e_PacketInterpreter::PACKET_NONE;

		/// <summary>
		/// What type of action is this packet?:
		/// SOFT   : Lowest priority action.
		/// MEDIUM : Will wipe all other soft actions in the queue.
		/// HARD   : Will wipe all actions with exception to hard.
		/// </summary>
		e_Action action = e_Action::SOFT_ACTION;

		template <class Archive>
		void serialize(Archive& ar)
		{	
			ar(interpreter);
			ar(action);
		}

		virtual ~s_PacketHeader() = default;
	};

	struct s_ActionPacket : public s_PacketHeader 
	{
		/// <summary>
		/// The entity the client wants to perform a action on.
		/// </summary>
		uint64_t entityId = -1;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
		}
	};

	struct s_Message : public s_ActionPacket 
	{
		std::string author  = "";
		std::string message = "";

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_ActionPacket>(this));
			ar(author);
			ar(message);
		}
	};

	struct s_NameChange : public s_ActionPacket
	{
		std::string name = "";

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_ActionPacket>(this));
			ar(name);
		}
	};

	struct s_TeleportEntity : public s_ActionPacket
	{
		int32_t x, y;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_ActionPacket>(this));
			ar(x);
			ar(y);
		}
	};

	struct s_HideEntity : public s_ActionPacket 
	{
		bool bShouldHide = false;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_ActionPacket>(this));
			ar(bShouldHide);
		}
	};

	struct s_CreateEntity : public s_PacketHeader
	{
		/// <summary>
		/// Pass connection Id if it's from client to server, else the server will pass the playerId instead towards the client.
		/// This id is either the playerUUID or the Client UUID
		/// </summary>
		uint64_t entityId = -1;
		
		/// <summary>
		/// What type of entity is this, Use the NPCDef as reference on clientside.
		/// 0 defaults to player.
		/// </summary>
		uint64_t npcId = 0;

		/// <summary>
		/// If this NPC should be hidden.
		/// </summary>
		bool bIsHidden = false;

		/// <summary>
		/// Where to spawn the entity.
		/// </summary>
		uint32_t posX = 0, posY = 0;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
			ar(npcId);
			ar(bIsHidden);
			ar(posX);
			ar(posY);
		}
	};

	struct s_EntityFollow : public s_PacketHeader
	{
		/// <summary>
		/// Pass connection Id if it's from client to server, else the server will pass the playerId instead towards the client.
		/// This id is either the playerUUID or the Client UUID
		/// </summary>
		uint64_t entityId = 0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
		}
	};

	struct s_UpdateSkill : public s_PacketHeader 
	{
		uint64_t entityId     =  0;
		uint8_t  skillType    = -1;
		int32_t  level        = -1;
		int32_t  levelBoosted = -1;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
			ar(skillType);
			ar(level);
			ar(levelBoosted);
		}
	};

	struct s_EntityHit : public s_PacketHeader
	{
		uint64_t fromEntityId = 0;
		uint64_t toEntityId   = 0;
		int32_t  hitAmount    = -1;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(fromEntityId);
			ar(toEntityId);
			ar(hitAmount);
		}
	};

	struct s_EntityMovement : public s_PacketHeader
	{
		/// <summary>
		/// The UUID of the entity, From Server->Client it's always the PlayerID, From Client->Server it's the client's
		/// Peer connect id;
		/// </summary>
		uint64_t entityId = -1;

		int  x = 0, y = 0;
		bool isRunning = false;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
			ar(x, y, isRunning);
		}
	};

	struct s_EntityPosition : public s_PacketHeader
	{
		/// <summary>
		/// The UUID of the entity, From Server->Client it's always the PlayerID, From Client->Server it's the client's
		/// Peer connect id;
		/// </summary>
		uint64_t entityId = -1;

		int x = 0 , y = 0;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
			ar(x, y);
		}
	};
}

