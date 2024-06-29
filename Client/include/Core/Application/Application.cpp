#include "precomp.h"

#include "Core/Application/Application.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/Layer/EntityLayer/EntityLayer.h"

#include "Core/UI/Layer/WorldLayer/WorldLayer.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

Application::Application() 
{
	m_bIsrunning = true;

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
	load_sprites();

	//Create 2D layers for interaction.
	create_layers();

	m_cursor = std::make_unique<RSCross>(Utilities::vec2{ 32.0f, 32.0f });

	//Initialize all layers.
	for (const auto& layer : m_layers)
	{
		layer->init();
	}
}

void Application::load_sprites()
{
	std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

	//TODO: Remove all of this and move it to a static const map.
	// Miscelaneous sprites
	{
		renderer->load_and_bind_surface("player/Player_Sheet.png", Graphics::SpriteType::PLAYER, 28);
		renderer->load_and_bind_surface("player/rank_owner.png", Graphics::SpriteType::PLAYER_RANK_OWNER);
		renderer->load_and_bind_surface("Tile.png",  Graphics::SpriteType::TILE_DEFAULT);
		renderer->load_and_bind_surface("Cross.png", Graphics::SpriteType::CROSS, 14);
	}

	// Load in all HUD elements.
	{
		renderer->load_and_bind_surface("hud/backdrop.png", Graphics::SpriteType::HUD_BACKDROP);
		renderer->load_and_bind_surface("hud/tabs/tab.png", Graphics::SpriteType::HUD_TAB);
		renderer->load_and_bind_surface("hud/frame.png",    Graphics::SpriteType::HUD_FRAME);
		renderer->load_and_bind_surface("hud/box.png",      Graphics::SpriteType::HUD_OPTIONS_BOX);
		renderer->load_and_bind_surface("hud/chatbox.png",  Graphics::SpriteType::HUD_CHATBOX);

		renderer->load_and_bind_surface("hud/skill/skillcontainer.png", Graphics::SpriteType::SKILL_BACKGROUND);
	}

	// Load in misc.
	{
		renderer->load_and_bind_surface("hitsplat.png", Graphics::SpriteType::HITSPLAT);
	}

	// Load in all icons.
	{
		renderer->load_and_bind_surface("hud/tabs/icons/default.png", Graphics::SpriteType::HUD_ICON_PLACEHOLDER);
		renderer->load_and_bind_surface("hud/tabs/icons/skills.png", Graphics::SpriteType::HUD_ICON_SKILLS);
		renderer->load_and_bind_surface("hud/tabs/icons/combat.png", Graphics::SpriteType::HUD_ICON_COMBAT);
		renderer->load_and_bind_surface("hud/tabs/icons/inventory.png", Graphics::SpriteType::HUD_ICON_INVENTORY);
	}

	//Load in all entities
	{
		renderer->load_and_bind_surface("entity/0.png", Graphics::SpriteType::NPC_GOBLIN, 50);
		renderer->load_and_bind_surface("entity/1.png", Graphics::SpriteType::NPC_INU, 18);
	}
}

void Application::create_layers()
{
	m_layers.push_back(std::make_unique<Graphics::UI::HUDLayer>());
	m_layers.push_back(std::make_unique<Graphics::UI::EntityLayer>());
	m_layers.push_back(std::make_unique<Graphics::UI::WorldLayer>());
}

void Application::update()
{
	//Update layers in reverse order, we want the first layer to be able to render ontop of everything else.
	for(auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
	{
		(*it)->update();
	}
	
	//Handle any events that SDL passes onto us.
	SDL_Event e;
	if(SDL_PollEvent(&e) != 0)
	{
		bool handledEvent = false;

		for (const auto& layer : m_layers)
		{
			// Handle input events
			if (layer->handle_event(&e))
			{
				handledEvent = true;
				break;
			}
		}

		// Want to move this somewhere else, possibly redo the renderer so registered sprites always render.
		{
			const bool clickedLeftMouse = e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT;
			if (clickedLeftMouse)
			{
				// Get the current mouse coordinates
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				const e_InteractionType interaction = handledEvent ? e_InteractionType::RED_CLICK : e_InteractionType::YELLOW_CLICK;
				m_cursor->click(Utilities::vec2((float)mouseX, (float)mouseY), interaction);
			}
		}

		switch(e.type) 
		{
		case SDL_MOUSEBUTTONDOWN:
		{
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				OptionsTab::open_option_menu();
			}
		}
		break;

		case SDL_WINDOWEVENT:
			//Handle viewport resizing.
			if(e.window.m_event == SDL_WINDOWEVENT_RESIZED) 
			{
				Utilities::ivec2 viewportSize;
				viewportSize.x = e.window.data1;
				viewportSize.y = e.window.data2;
				g_globals.renderer.lock()->on_viewport_size_changed.invoke(Utilities::to_vec2(viewportSize));
			}
			break;

			case SDL_QUIT:
			close_application();
			return;
		}
	}

	m_cursor->update();

	Graphics::Animation::Animator::update();
}

const bool Application::is_running() const
{
	return m_bIsrunning;
}

void Application::close_application()
{
	//Send packet to de-register the player from the server.
		//Send out the packet.
	const std::shared_ptr<ENetPacketHandler> packetHandler = g_globals.packetHandler.lock();

	Packets::s_PacketHeader packet;
	packet.interpreter = e_PacketInterpreter::PACKET_REMOVE_ENTITY;

	packetHandler->send_packet<Packets::s_PacketHeader>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);

	m_bIsrunning = false;
}