#include "precomp.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"


_ClientInfo::_ClientInfo()
{
	peer = nullptr;
	playerId    = 0;
	idleticks   = 0;
	packetquery = nullptr; 
}

_ClientInfo::~_ClientInfo()
{
	peer = nullptr;
	delete packetquery;
}

void _ClientInfo::reset_idle_timer()
{
	idleticks = 0;
}
