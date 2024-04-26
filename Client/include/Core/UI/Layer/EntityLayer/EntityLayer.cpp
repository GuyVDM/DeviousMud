#include "precomp.h"

#include "Core/UI/Layer/EntityLayer/EntityLayer.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

#include <functional>

void Graphics::UI::EntityLayer::init()
{
	//Set the player camera as the main camera within the renderer
	{
		m_renderer = g_globals.renderer.lock();
		m_playerCamera = std::make_shared<Camera>();
		m_renderer->set_camera(m_playerCamera);
	}

	//Grab the player sprite from the renderer and cache it for rendering.
	{
		m_entityHandler = g_globals.entityHandler.lock();
	}

	//Bind listeners
	{
		DEVIOUS_LOG("Binding PlayerLayer callbacks to Playerhandler...");

		m_entityHandler->on_local_player_assigned.add_listener
		(
			std::bind(&EntityLayer::player_local_assigned, this, std::placeholders::_1)
		);
	}
}

void Graphics::UI::EntityLayer::update()
{
	for(const auto& worldEntity : m_entityHandler->m_worldEntities) 
	{
		const RefEntity& entity = worldEntity.second;
		const SimPosition& entitySim = entity->get_simulated_data();
		const Utilities::vec2 tileCenterOffset = Utilities::vec2(1.0f) - Utilities::vec2(0.25f, 0.15f);
		m_renderer->plot_frame(entity->get_sprite(), entitySim.get_position() - tileCenterOffset, m_entitySize * entity->get_definition().size);
	}

	if (m_bHasLocalPlayer)
	{
		//Set camera position to be the player
		const SimPosition& localPlayer = m_entityHandler->get_local_player_data()->get_simulated_data();

		const Utilities::vec2 transformedPos
		{
			localPlayer.get_position().x * (float)Renderer::GRID_CELL_PX_SIZE,
			localPlayer.get_position().y * (float)Renderer::GRID_CELL_PX_SIZE
		};

		m_playerCamera->set_position(transformedPos);
	}
}

bool Graphics::UI::EntityLayer::handle_event(const SDL_Event* _event)
{
	for (const auto& worldEntity : m_entityHandler->m_worldEntities)
	{
		const RefEntity entity = worldEntity.second;
		
		if(entity->handle_event(_event)) 
		{
			return true;
		}
	}

	return false;
}

void Graphics::UI::EntityLayer::player_local_assigned(uint64_t _playerId)
{
	m_bHasLocalPlayer = true;
}
