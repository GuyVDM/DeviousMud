#pragma once
#include <memory>

#pragma region FORWARD_DECLERATIONS
class NetworkHandler;

namespace Server 
{
	class ConnectionHandler;
	class EntityHandler;
	class World;
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
	std::shared_ptr<Server::World>             world;
};

extern Globals g_globals;
