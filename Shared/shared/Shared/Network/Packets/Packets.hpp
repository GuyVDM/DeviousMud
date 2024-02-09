#pragma once
#include <fstream>
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"

enum e_PacketInterpreter : uint8_t
{
	PACKET_NONE                 = 0x00,

	PACKET_PING                 = 0x01,

	PACKET_CREATE_PLAYER        = 0x02,

	PACKET_ASSIGN_LOCAL_PLAYER  = 0x03,

	PACKET_DISCONNECT_PLAYER    = 0x04,

	PACKET_MOVE_PLAYER          = 0x05,

	PACKET_TIMEOUT_WARNING      = 0x06
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

	struct s_Player : public s_PacketHeader
	{
		/// <summary>
		/// Pass connection Id if it's from client to server, else the server will pass the playerId instead towards the client.
		/// </summary>
		uint64_t playerId = -1;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_PacketHeader>(this));
			ar(playerId);
		}
	};

	struct s_PlayerMovement : public s_Player
	{
		int  x = 0, y = 0;
		bool isRunning = false;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::base_class<s_Player>(this));
			ar(x, y, isRunning);
		}
	};

	struct s_PlayerPosition : public s_Player
	{
		int x = 0 , y = 0;

		template<class Archive>
		void serialize(Archive& ar) 
		{
			ar(cereal::base_class<s_Player>(this));
			ar(x, y);
		}
	};
}

