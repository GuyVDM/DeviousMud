#pragma once
#include <enet/enet.h>
#include <enet/types.h>

#include "cereal/archives/portable_binary.hpp"
#include "Packets.hpp"

#include <type_traits>

#include "../shared/Shared/Utilities/Assert.h"

class PacketHandler
{
#define PACKET_SENT 0;
#define PACKET_ERR  1;

public:
	template<class T>
	constexpr static int send_packet(T* _data, ENetPeer* _peer, ENetHost* _host, enet_uint8 _channel, enet_uint32 _flags);

	template<class T>
	constexpr static void send_packet_multicast(T* _data, ENetHost* _host, enet_uint8 _channel, enet_uint32 _flags);

	template<class T>
	constexpr static void retrieve_packet_data(T& _packet, ENetEvent* _e);
};

/// <summary>
/// Returns 0 on success, < 0 on failure.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="eventtype"></param>
/// <param name="data"></param>
/// <param name="channel"></param>
/// <param name="flags"></param>
/// <returns></returns>
template<class T>
constexpr inline int PacketHandler::send_packet(T* _data, ENetPeer* _peer, ENetHost* _host, enet_uint8 _channel, enet_uint32 _flags)
{	
	//Check if the peer id is registered.
	if (_peer->connectID > 0)
	{
		static_assert(std::is_base_of<Packets::s_PacketHeader, T>::value || std::is_same<Packets::s_PacketHeader, T>::value, "T must inherit from the packetheader.");

		std::ostringstream os;
		{
			cereal::PortableBinaryOutputArchive ar(os);
			ar(*_data);
		}

		std::string stringdata = os.str();
		ENetPacket* packet = enet_packet_create(stringdata.data(), stringdata.size(), _flags);

		if (enet_peer_send(_peer, _channel, packet) == 0)
		{
			enet_host_flush(_host);
		}
		else
		{
			DEVIOUS_ASSERT(_peer != nullptr);
			DEVIOUS_ASSERT(packet != nullptr);
			DEVIOUS_ERR("Something went wrong with sending out a packet...");
		}

		return PACKET_SENT;
	}
	
	return PACKET_ERR;
}

template<class T>
constexpr inline void PacketHandler::send_packet_multicast(T* _data, ENetHost* _host, enet_uint8 _channel, enet_uint32 _flags)
{
	for(int i = 0; i < _host->peerCount; i++)
	{
		ENetPeer* peer = &_host->peers[i];
		send_packet<T>(_data, peer, _host, _channel, _flags);
	}
}

template<class T>
constexpr inline void PacketHandler::retrieve_packet_data(T& _packet, ENetEvent* _e)
{
	static_assert(std::is_base_of<Packets::s_PacketHeader, T>::value || std::is_same<Packets::s_PacketHeader, T>::value, "T must inherit from the packetheader.");

	try
	{
		std::string st((const char*)_e->packet->data, _e->packet->dataLength);

		std::istringstream is(st);
		{
			cereal::PortableBinaryInputArchive ar(is);
			ar(_packet);
		}
	}
	catch (const std::exception& e)
	{
		DEVIOUS_ERR("Serialization error: " << e.what() << std::endl);
	}
}
