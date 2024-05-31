#include "precomp.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"


ClientInfo::ClientInfo()
{
	peer		  = nullptr;
	playerId      = 0;
	idleticks     = 0;
	clientId      = 0;
	bAwaitingPing = false;
	packetquery   = nullptr; 
}

ClientInfo::~ClientInfo()
{
	peer = nullptr;
	delete packetquery;
}

void ClientInfo::refresh()
{
	bAwaitingPing = false;
	ticksSinceLastResponse = 0;
	idleticks = 0;
}
