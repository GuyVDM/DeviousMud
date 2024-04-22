#include "precomp.h"

#include "Core/UI/Layer/EntityLayer/EntityLayer.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

#include <functional>

void Graphics::UI::EntityLayer::init()
{
	//Set player size.
	{
		m_playerSize = Utilities::vec2(128.0f);
	}

	//Set the player camera as the main camera within the renderer
	{
		m_renderer = g_globals.m_renderer.lock();
		m_playerCamera = std::make_shared<Camera>();
		m_renderer->set_camera(m_playerCamera);
	}

	//Grab the player sprite from the renderer and cache it for rendering.
	{
		entityHandler = g_globals.entityHandler.lock();
	}

	//Bind listeners
	{
		DEVIOUS_LOG("Binding PlayerLayer callbacks to Playerhandler...");

		entityHandler->on_local_player_assigned.add_listener
		(
			std::bind(&EntityLayer::player_local_assigned, this, std::placeholders::_1)
		);

		entityHandler->on_player_created.add_listener
		(
			std::bind(&EntityLayer::player_created, this, std::placeholders::_1)
		);

		entityHandler->on_player_removed.add_listener
		(
			std::bind(&EntityLayer::player_destroyed, this, std::placeholders::_1)
		);
	}
}

void Graphics::UI::EntityLayer::update()
{
	//Render all player sprites.
	for (const uint64_t playerHandle : m_playerHandles)
	{
		PlayerData& playerData = entityHandler->get_data(playerHandle);
		SimPosition& m_simPos = playerData.m_simPos;

		//Update the simulated player position.
		if (m_simPos.is_dirty())
		{
			m_simPos.update();
		}

		const Utilities::vec2 tileCenterOffset = (((m_playerSize * 0.5f) / 64.0f) - Utilities::vec2(0.25f, 0.15f));
		m_renderer->plot_frame(playerData.m_sprite, m_simPos.get_position() - tileCenterOffset, m_playerSize);
	}

	if (m_bHasLocalPlayer)
	{
		//Set camera position to be the player
		SimPosition& localPlayer = entityHandler->get_local_player_data().m_simPos;

		const Utilities::vec2 transformedPos
		{
			localPlayer.get_position().x * (float)Renderer::GRID_CELL_PX_SIZE,
			localPlayer.get_position().y * (float)Renderer::GRID_CELL_PX_SIZE
		};

		m_playerCamera->set_position(transformedPos);
	}
}

void Graphics::UI::EntityLayer::player_created(uint64_t _playerId)
{
	m_playerHandles.push_back(_playerId);
}

void Graphics::UI::EntityLayer::player_destroyed(uint64_t _playerId)
{
	auto it = std::find(m_playerHandles.begin(), m_playerHandles.end(), _playerId);
	m_playerHandles.erase(it);
}

void Graphics::UI::EntityLayer::player_local_assigned(uint64_t _playerId)
{
	m_bHasLocalPlayer = true;
}
