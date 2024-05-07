#include "precomp.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Core/Network/NetworkHandler.h"

#include "Core/Config/Config.h" //TODO: This needss to be moved to a shared project at some point.

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Events/Handler/EventHandler.h"

#include "Core/Game/World/World.h"

#include "Core/Globals/S_Globals.h"

Globals g_globals;

NetworkHandler NetworkHandler::create_local_host(int32_t _maxconnections, int32_t _channels, int32_t _inc_bandwith, int32_t _outg_bandwidth)
{
	const char* ADRESS = "127.0.0.1";
	constexpr static int32_t PORT = 1234;
	return create_host(ADRESS, PORT, _maxconnections, _channels, _inc_bandwith, _outg_bandwidth);
}

NetworkHandler NetworkHandler::create_host(const char* _address, int32_t _port, int32_t _maxconnections, int32_t _channels, int32_t _inc_bandwith, int32_t _outg_bandwidth)
{
	//Set server host & Connection parameters
	ENetAddress connection;
	ENetHost*   server;

	enet_address_set_host(&connection, _address);
	connection.port = _port;

	server = enet_host_create(&connection,
							   _maxconnections,
							   _channels,
							   _inc_bandwith,
							   _outg_bandwidth);

	if (server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create a ENet Server Host. \n");
		exit(EXIT_FAILURE);
	}

	DEVIOUS_EVENT("Server listening on: [" << _address << ":" << _port << "].");
	return NetworkHandler(server);
}

ENetHost* NetworkHandler::get_server_host()
{
	return m_server;
}

void NetworkHandler::start_ticking()
{
	auto connectionHandler = std::make_shared<Server::ConnectionHandler>();
	auto entityHandler     = std::make_shared<Server::EntityHandler>();
	auto world = std::make_shared<Server::World>();

	bool is_running = true;

	//Setting global references.
	g_globals.connectionHandler = connectionHandler;
	g_globals.entityHandler		= entityHandler;
	g_globals.networkHandler    = std::shared_ptr<NetworkHandler>(this);
	g_globals.world             = world;

	world->init();

	float ticktimer = 0.0f;

	while(is_running)
	{
		DM::SERVER::Config::update_deltaTime();
		ticktimer += DM::SERVER::Config::get_deltaTime();

		ENetEvent e;

		if(enet_host_service(m_server, &e, 0) > 0) //Try and catch packets before handling them
		{
			switch (e.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				connectionHandler->register_client(e.peer);
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				connectionHandler->disconnect_client(e.peer->connectID);
			}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				RefClientInfo clientInfo = connectionHandler->get_client_info(e.peer->connectID);
				Server::EventHandler::queue_incoming_event(&e, clientInfo);
				enet_packet_destroy(e.packet);
			}
			break;
			}
		}

		if(ticktimer > m_tickDuration) 
		{
			ticktimer = 0.0f;
			Server::EventHandler::handle_queud_events(m_server);

			//Check for timeouts.
			connectionHandler->update_idle_timers();
			entityHandler->entities_tick();
		}
	}
}

void NetworkHandler::destroy()
{
	delete this;
}

NetworkHandler::~NetworkHandler()
{
	enet_host_destroy(m_server);
}

NetworkHandler::NetworkHandler(ENetHost* _server)
{
	this->m_server = _server;
}
