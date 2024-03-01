#pragma once
#include <memory>
#include "Shared/Utilities/UUID.hpp"

#pragma region FORWARD_DECLERATIONS

class EventQuery;

typedef struct _ENetPeer ENetPeer;

typedef unsigned int enet_uint32;

#pragma endregion

/// <summary>
/// Struct containing all info regarding the client.
/// NOTE: DON'T USE ANY PEER RELATED VARIABLES, THEY'RE ALL VOLATILE.
/// </summary>
typedef struct _ClientInfo 
{
	ENetPeer*				  peer;                    //Client peer, Identified by 'peer->connectId' 
	enet_uint32               clientId;
	DEVIOUSMUD::RANDOM::UUID  playerId;                //Unique UUID which refers to its player in the player handler.
	EventQuery*               packetquery;             //Player specific query for packets.

	bool                      bAwaitingPing;           //Whether the server is waiting for the client to respond to a sent out ping to see if there's still a valid connection.
	uint32_t                  ticksSinceLastResponse;  //How many ticks have elapsed since the server has last received a ping from the client.
	uint32_t	              idleticks;               //Time since the player has last done any interaction.

	/// <summary>
	/// When called, will reset any bound timers related to its activity.
	/// </summary>
	void refresh();

	_ClientInfo();
	~_ClientInfo();

}	ClientInfo;