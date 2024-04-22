#include "precomp.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"


ClientInfo::ClientInfo()
{
	peer = nullptr;
	fromPlayerId    = 0;
	idleticks   = 0;
	packetquery = nullptr; 
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
