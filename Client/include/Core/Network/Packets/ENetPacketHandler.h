#pragma once
#include <memory>

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Utilities/EventListener.h"

#include <enet/enet.h>

class Application;

/// <summary>
/// Interprets any packets that gets received.
/// </summary>
class ENetPacketHandler 
{
public:
	EventListener<void> on_disconnect;

	template<class T>
	void send_packet(T* data, enet_uint8 channel, enet_uint32 flags);

	void update();

public:
	ENetPacketHandler(ENetHost* _host, ENetPeer* _peer);
	~ENetPacketHandler();

private:
	void process_packet();

private:
	ENetHost*				   m_host;
	ENetPeer*				   peer;
	ENetEvent				   m_event;
};

template<class T>
inline void ENetPacketHandler::send_packet(T* data, enet_uint8 channel, enet_uint32 flags)
{
	PacketHandler::send_packet<T>(data, peer, m_host, channel, flags);
}
