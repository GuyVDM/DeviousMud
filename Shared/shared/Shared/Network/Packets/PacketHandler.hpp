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
	constexpr static int send_packet(T* data, ENetPeer* m_peer, ENetHost* m_host, enet_uint8 channel, enet_uint32 flags);

	template<class T>
	constexpr static void send_packet_multicast(T* data, ENetHost* m_host, enet_uint8 channel, enet_uint32 flags);

	template<class T>
	constexpr static void retrieve_packet_data(T& packet, ENetEvent* ev);
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
constexpr inline int PacketHandler::send_packet(T* data, ENetPeer* m_peer, ENetHost* m_host, enet_uint8 channel, enet_uint32 flags)
{	
	//Check if the peer id is registered.
	if (m_peer->connectID > 0) 
	{
		static_assert(std::is_base_of<Packets::s_PacketHeader, T>::value || std::is_same<Packets::s_PacketHeader, T>::value, "T must inherit from the packetheader.");

		std::ostringstream os;
		{
			cereal::PortableBinaryOutputArchive ar(os);
			ar(*data);
		}

		std::string stringdata = os.str();
		ENetPacket* packet = enet_packet_create(stringdata.c_str(), sizeof(stringdata), flags);

		if (enet_peer_send(m_peer, channel, packet) == 0)
		{
			enet_host_flush(m_host);
		}
		else
		{
			DEVIOUS_ASSERT(m_peer != nullptr);
			DEVIOUS_ASSERT(packet != nullptr);
			fprintf(stderr, "Something went wrong with sending out a packet... \n");
		}

		return PACKET_SENT;
	}
	
	return PACKET_ERR;
}

template<class T>
inline constexpr void PacketHandler::send_packet_multicast(T* data, ENetHost* m_host, enet_uint8 channel, enet_uint32 flags)
{
	for(int i = 0; i < m_host->peerCount; i++)
	{
		ENetPeer* client = &m_host->peers[i];
		send_packet<T>(data, client, m_host, channel, flags);
	}
}

template<class T>
constexpr inline void PacketHandler::retrieve_packet_data(T& packet, ENetEvent* ev)
{
	static_assert(std::is_base_of<Packets::s_PacketHeader, T>::value || std::is_same<Packets::s_PacketHeader, T>::value, "T must inherit from the packetheader.");

	std::string st((const char*)ev->packet->data, ev->packet->dataLength);
	
	std::istringstream is(st);
	{
		cereal::PortableBinaryInputArchive ar(is);
		ar(packet);
	}
}
