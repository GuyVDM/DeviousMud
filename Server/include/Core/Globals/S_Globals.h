#pragma once
#include <memory>

#pragma region FORWARD_DECLERATIONS
class NetworkHandler;

namespace Server 
{
	class ConnectionHandler;
	class EntityHandler;
}


#pragma endregion

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
struct Globals
{
	std::shared_ptr<Server::ConnectionHandler> connectionHandler;
	std::shared_ptr<Server::EntityHandler>     entityHandler;
	std::shared_ptr<NetworkHandler>            networkHandler;
};

extern Globals g_globals;
