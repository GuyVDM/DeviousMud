#pragma once
#include <memory>

#pragma region FORWARD_DECLERATIONS
class NetworkHandler;

namespace Server 
{
	class ConnectionHandler;
	class PlayerHandler;
}


#pragma endregion

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
typedef struct _Globals
{
	std::shared_ptr<Server::ConnectionHandler> connectionHandler;
	std::shared_ptr<Server::PlayerHandler>     playerHandler;
	std::shared_ptr<NetworkHandler>            networkHandler;
}   Globals;

extern Globals g_globals;
