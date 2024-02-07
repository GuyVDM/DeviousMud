#pragma once
#include <memory>
#include "Shared/Utilities/UUID.hpp"

#pragma region FORWARD_DECLERATIONS

class EventQuery;

typedef struct _ENetPeer ENetPeer;

typedef unsigned int enet_uint32;

#pragma endregion

typedef struct _ClientInfo 
{
	ENetPeer*				 peer; //Identified by 'peer->connectId'
	DEVIOUSMUD::RANDOM::UUID playerId;
	uint32_t	             idleticks;
	EventQuery*              packetquery;

	/// <summary>
	/// Resets the idle timer to 0.
	/// </summary>
	void reset_idle_timer();

	_ClientInfo();
	~_ClientInfo();

}	ClientInfo;