#include "precomp.h"

#include "Core/Application/Application.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/UI/Layer/PlayerLayer/PlayerLayer.h"

#include "Core/UI/Layer/WorldLayer/WorldLayer.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

Application::Application() 
{
	bIsrunning = true;

	std::shared_ptr<ENetPacketHandler> packetHandler = g_globals.packetHandler.lock();

	//Add listener that if we get timed out, close the program.
	packetHandler->on_disconnect.add_listener
	(
		std::bind(&Application::close_application, this)
	);
}

void Application::init()
{
	//Load in all textures
	load_textures();

	//Create 2D layers for interaction.
	create_layers();

	//Initialize all layers.
	for (const auto& layer : layers)
	{
		layer->init();
	}
}

void Application::load_textures()
{
	std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

	//--Set texture file path---//
	renderer->load_and_bind_surface("Tile.png", Graphics::SpriteType::TILE_DEFAULT, 1);
	renderer->load_and_bind_surface("Kirby.png", Graphics::SpriteType::PLAYER, 1);
}

void Application::create_layers()
{
	//World Layer creation
	layers.push_back(std::make_unique<Graphics::UI::WorldLayer>());
	layers.push_back(std::make_unique<Graphics::UI::PlayerLayer>());
}

void Application::update()
{
	//Update layers
	for(const auto& layer : layers) 
	{
		layer->update();
	}

	SDL_Event e;
	
	//Handle any events that SDL passes onto us.
	if(SDL_PollEvent(&e) != 0)
	{
		for (const auto& layer : layers)
		{
			// Handle input events
			if (layer->handle_event(&e))
			{
				break;
			}
		}

		switch(e.type) 
		{
			case SDL_QUIT:
			close_application();
			return;
		}
	}
}

const bool Application::is_running() const
{
	return bIsrunning;
}

void Application::close_application()
{
	//Send packet to de-register the player from the server.
		//Send out the packet.
	const std::shared_ptr<ENetPacketHandler> packetHandler = g_globals.packetHandler.lock();

	Packets::s_PacketHeader packet;
	packet.interpreter = e_PacketInterpreter::PACKET_DISCONNECT_PLAYER;

	packetHandler->send_packet<Packets::s_PacketHeader>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);

	bIsrunning = false;
}