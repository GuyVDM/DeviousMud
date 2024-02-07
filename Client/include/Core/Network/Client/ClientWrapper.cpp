#include "precomp.h"

#include "ClientWrapper.h"

#include "Core/Global/C_Globals.h" //Client specific global references

#include "Core/Application/Application.h"

#include "Core/Application/Config/Config.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Player/PlayerHandler.h"

#include "Core/UI/Layer/Layer.h"

#include "Shared/Utilities/vec3.hpp"

#include <random>

Globals g_globals;

Client::Client(ENetHost* c)
{
	host = c;
	peer = nullptr;
}

Client Client::connect_localhost(bool& succeeded)
{
	const char* ip = "127.0.0.1";
	int32_t	port = 1234;
	return connect_host(succeeded, ip, port);
}

Client Client::connect_host(bool& succeeded, const char* ip, int32_t port)
{
	const enet_uint32 CONNECTION_TIME_MS = 5000;

	ENetHost* host;
	ENetAddress address;
	succeeded = false;

	host = enet_host_create(NULL,
		1,
		2,
		0,
		0);

	Client c(host);

	if (host == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}

	enet_address_set_host(&address,ip);
	address.port = port;

	c.peer = enet_host_connect(host, &address, 2, 0);

	if (c.peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}

	/* Wait up to 5 seconds for the connection attempt to succeed.*/
	ENetEvent e;
	if (enet_host_service(host, &e, CONNECTION_TIME_MS) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
	{
		succeeded = true;
		puts("Connection to 127.0.0.1:1234 succeeded.\n");
		c.init();
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(c.peer);
		puts("Connection to 127.0.0.1:1234 failed.\n");
	}

	return c;
}

void Client::init()
{
	//Initialise SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
}

void Client::quit()
{
	//Clean up ENet
	enet_peer_reset(peer);
	enet_deinitialize();
	enet_host_destroy(host);

	//Clear up SDL
	IMG_Quit();
	SDL_Quit();

	printf("Shutting down.\n");
}

void Client::start_ticking()
{
	//Camera creation
	const uint32_t render_distance = 100;
	auto camera = std::make_shared<Camera>(render_distance);

	//Packethandler creation.
	auto packetHandler = std::make_shared<ENetPacketHandler>(host, peer);

	//Playerhandler creation.
	auto playerHandler = std::make_shared<PlayerHandler>();

	//Renderer creation
	const std::string texture_path = "assets/sprites/";
	const Utilities::ivec2 window_size = Utilities::ivec2(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	auto renderer = std::shared_ptr<Graphics::Renderer>
	(
		Graphics::Renderer::create_renderer(TITLE, window_size, texture_path)
	);

	//Setup globals
	g_globals.camera = camera;
	g_globals.renderer = renderer;
	g_globals.packetHandler = packetHandler;
	g_globals.playerHandler = playerHandler;

	//Create application
	auto application = std::make_shared<Application>();
	application->init();

	while (application->is_running())
	{
		DEVIOUSMUD::CLIENT::Config::update_deltaTime();


		renderer->start_frame();

		packetHandler->update();

		application->update();

		renderer->end_frame();
	}
}
