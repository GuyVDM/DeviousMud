#include "precomp.h"

#include "Core/UI/Layer/PlayerLayer/PlayerLayer.h"

#include "Core/Player/PlayerHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include <functional>

void Graphics::UI::PlayerLayer::init()
{
	weakPlayerHandler = g_globals.playerHandler;

	playerSprite = renderer.lock()->create_sprite_from_surface(SpriteType::PLAYER);

	std::shared_ptr<PlayerHandler> playerHandler = weakPlayerHandler.lock();

	std::cout << "[Layers::PlayerLayer] Binding callbacks to Playerhandler..." << std::endl;

	//Bind listeners
	playerHandler->on_player_created.add_listener
	(
		std::bind(&PlayerLayer::player_created, this, std::placeholders::_1)
	);

	playerHandler->on_player_removed.add_listener
	(
		std::bind(&PlayerLayer::player_destroyed, this, std::placeholders::_1)
	);
}

void Graphics::UI::PlayerLayer::update()
{
	const std::shared_ptr<Renderer> r = renderer.lock();

	for(const uint64_t playerHandle : playerHandles) 
	{
		const PlayerDetails details = weakPlayerHandler.lock()->get_details(playerHandle);
		const Utilities::ivec2 size{ 32, 32 };

		const Utilities::ivec2 transformedPos
		{
			details.position.x * Renderer::GRID_CELL_SIZE_PX,
			details.position.y * Renderer::GRID_CELL_SIZE_PX
		};


		r->plot_frame(playerSprite, transformedPos, size);
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
