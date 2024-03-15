#include "precomp.h"

#include "Core/UI/Layer/PlayerLayer/PlayerLayer.h"

#include "Core/Player/PlayerHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/Global/C_Globals.h"

#include <functional>

void Graphics::UI::PlayerLayer::init()
{
	//Set the player camera as the main camera within the renderer
	{
		renderer = g_globals.renderer.lock();
		playerCamera = std::make_shared<Camera>();
		renderer->set_camera(playerCamera);
	}

	//Grab the player sprite from the renderer and cache it for rendering.
	{
		playerHandler = g_globals.playerHandler.lock();
		playerSprite = renderer->get_sprite(SpriteType::PLAYER);
	}

	//Bind listeners
	{
		DEVIOUS_LOG("Binding PlayerLayer callbacks to Playerhandler...");

		playerHandler->on_local_player_assigned.add_listener
		(
			std::bind(&PlayerLayer::player_local_assigned, this, std::placeholders::_1)
		);

		playerHandler->on_player_created.add_listener
		(
			std::bind(&PlayerLayer::player_created, this, std::placeholders::_1)
		);

		playerHandler->on_player_removed.add_listener
		(
			std::bind(&PlayerLayer::player_destroyed, this, std::placeholders::_1)
		);
	}
}

void Graphics::UI::PlayerLayer::update()
{
	//Render all player sprites.
	for (const uint64_t playerHandle : playerHandles)
	{
		const Utilities::vec2 size{ 32.0f, 32.0f };

		SimPosition& simPos = playerHandler->get_data(playerHandle).simPos;

		//Update the simulated player position.
		if (simPos.is_dirty())
		{
			simPos.update();
		}

		renderer->plot_frame(playerSprite, simPos.get_position(), size);
	}

	if (bHasLocalPlayer)
	{
		//Set camera position to be the player
		SimPosition& localPlayer = playerHandler->get_local_player_data().simPos;

		const Utilities::vec2 transformedPos
		{
			localPlayer.get_position().x * (float)Renderer::GRID_CELL_PX_SIZE,
			localPlayer.get_position().y * (float)Renderer::GRID_CELL_PX_SIZE
		};

		playerCamera->set_position(transformedPos);
	}
}

void Graphics::UI::PlayerLayer::player_created(uint64_t _playerId)
{
	playerHandles.push_back(_playerId);
}

void Graphics::UI::PlayerLayer::player_destroyed(uint64_t _playerId)
{
	auto it = std::find(playerHandles.begin(), playerHandles.end(), _playerId);
	playerHandles.erase(it);
}

void Graphics::UI::PlayerLayer::player_local_assigned(uint64_t _playerId)
{
	bHasLocalPlayer = true;
}
