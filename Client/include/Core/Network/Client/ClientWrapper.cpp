#include "precomp.h"

#include "ClientWrapper.h"

#include "Core/Global/C_Globals.h"

#include "Core/Application/Application.h"

#include "Core/Application/Config/Config.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Util/TimerHandler.h"

#include "Core/UI/Layer/Layer.h"

#include "Shared/Utilities/vec3.hpp"

#include <random>

Globals g_globals;

Client::Client(ENetHost* c)
{
	m_host = c;
	m_peer = nullptr;
}

Client Client::connect_localhost(bool& _succeeded)
{
	const char* ip = "127.0.0.1";
	int32_t	port = 1234;
	return connect_host(_succeeded, ip, port);
}

Client Client::connect_host(bool& _succeeded, const char* _ip, const uint16_t _port)
{
	DEVIOUS_EVENT("Trying to connect to: [" << _ip << ":" << _port << "].");

	const enet_uint32 CONNECTION_TIME_MS = 5000;

	ENetHost* m_host;
	ENetAddress address;
	_succeeded = false;

	m_host = enet_host_create(NULL,
		1,
		2,
		0,
		0);

	Client c(m_host);

	if (m_host == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}

	enet_address_set_host(&address, _ip);
	address.port = _port;
	

	c.m_peer = enet_host_connect(m_host, &address, 2, 0);

	if (c.m_peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}

	//*------------------------------------
	// Wait for the connection to succeed.
	//
	ENetEvent e;
	if (enet_host_service(m_host, &e, CONNECTION_TIME_MS) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
	{
		_succeeded = true;
		DEVIOUS_EVENT("Connection to: [" << _ip << ":" << _port << "] succeeded.");
		c.init();
	}
	else
	{
		enet_peer_reset(c.m_peer);
		DEVIOUS_ERR("Failed to connect to: [" << _ip << ":" << _port << "].");
	}

	return c;
}

void Client::init()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
}

void Client::quit()
{
	//Clean up ENet
	enet_peer_reset(m_peer);
	enet_deinitialize();
	enet_host_destroy(m_host);

	//Clear up SDL
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	DEVIOUS_EVENT("Shutting down.");
}

void Client::start_ticking()
{
	//Packethandler creation.
	auto packetHandler = std::make_shared<ENetPacketHandler>(m_host, m_peer);

	//Entityhandler creation.
	auto entityHandler = std::make_shared<EntityHandler>();

	auto timerHandler = std::make_shared<TimerHandler>();

	//Renderer creation
	const std::string texture_path = "assets";
	const Utilities::ivec2 window_size = Utilities::ivec2(1000, 800);

	auto renderer = std::shared_ptr<Graphics::Renderer>
	(
		Graphics::Renderer::create_renderer(TITLE, window_size, texture_path)
	);

	//Setup globals
	{
		g_globals.renderer      = renderer;
		g_globals.packetHandler = packetHandler;
		g_globals.entityHandler = entityHandler;
		g_globals.timerHandler  = timerHandler;
	}

	//Create application
	auto application = std::make_shared<Application>();
	application->init();

	while (application->is_running())
	{
		DM::CLIENT::Config::update_deltaTime();

		renderer->start_frame();

		packetHandler->update();

		application->update();

		timerHandler->update();

		entityHandler->update();

		renderer->end_frame();
	}
}
