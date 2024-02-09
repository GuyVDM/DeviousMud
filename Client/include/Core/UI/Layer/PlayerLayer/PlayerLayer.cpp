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
		playerSprite = renderer->create_sprite_from_surface(SpriteType::PLAYER);
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
	if(bHasLocalPlayer) 
	{
		//Set camera position to be the player
		PlayerDetails& localPlayer = playerHandler->get_local_player_details();

		const Utilities::ivec2 transformedPos
		{
			localPlayer.position.x * Renderer::GRID_CELL_SIZE_PX,
			localPlayer.position.y * Renderer::GRID_CELL_SIZE_PX
		};

		playerCamera->set_position(Utilities::ivec3{ transformedPos.x, transformedPos.y, playerCamera->get_position().z });
	}

	//Render all player sprites.
	for(const uint64_t playerHandle : playerHandles) 
	{
		const PlayerDetails details = playerHandler->get_details(playerHandle);
		const Utilities::ivec2 size{ 32, 32 };

		renderer->plot_frame(playerSprite, details.position, size);
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
