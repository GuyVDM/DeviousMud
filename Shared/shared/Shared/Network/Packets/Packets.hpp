#pragma once
#include <fstream>
#include <vector>
#include <utility>
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"

enum e_PacketInterpreter : uint8_t
{
	PACKET_NONE                        = 0x00,

	PACKET_PING                        = 0x01,

	PACKET_CREATE_ENTITY               = 0x02,

	PACKET_ASSIGN_LOCAL_PLAYER_ENTITY  = 0x03,

	PACKET_REMOVE_ENTITY               = 0x04,

	PACKET_MOVE_ENTITY                 = 0x05,

	PACKET_TIMEOUT_WARNING             = 0x06,

	PACKET_PLAYER_PATH                 = 0x07,

	PACKET_FOLLOW_ENTITY               = 0x08
};

namespace Packets
{
	/// <summary>
	/// All further packets should implement this as their base with each packet their first bit representing the identifier.
	/// </summary>
	struct s_PacketHeader
	{
		e_PacketInterpreter interpreter = e_PacketInterpreter::PACKET_NONE;

		template <class Archive>
		void serialize(Archive& ar)
		{	
			ar(interpreter);
		}

		virtual ~s_PacketHeader() = default;
	};

	struct s_Entity : public s_PacketHeader
	{
		/// <summary>
		/// Pass connection Id if it's from client to server, else the server will pass the playerId instead towards the client.
		/// </summary>
		uint64_t fromEntityId = -1;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(fromEntityId);
		}
	};

	struct s_EntityFollow : public s_PacketHeader
	{
		/// <summary>
		/// Pass connection Id if it's from client to server, else the server will pass the playerId instead towards the client.
		/// </summary>
		uint64_t entityId = -1;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(entityId);
		}
	};

	struct s_EntityMovement : public s_Entity
	{
		int  x = 0, y = 0;
		bool isRunning = false;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_Entity>(this));
			ar(x, y, isRunning);
		}
	};

	struct s_EntityPath : public s_Entity 
	{
		int pathSize;
		std::vector<std::pair<int, int>> m_path;

		template <class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_Entity>(this));
			ar(pathSize);
			
			for(int i = 0; i < m_path.size(); i++) 
			{
				ar(m_path[i].first, m_path[i].second);
			}
		}
	};

	struct s_EntityPosition : public s_Entity
	{
		int x = 0 , y = 0;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_Entity>(this));
			ar(x, y);
		}
	};
}

