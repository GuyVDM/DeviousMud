#include "precomp.h"

#include "Core/Events/Clickable/Tile/WorldTile.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Global/C_Globals.h"

WorldTile::~WorldTile()
{
}

std::shared_ptr<WorldTile> WorldTile::create_tile(const Utilities::vec2& position)
{
	const Utilities::vec2 scale
	(
		Graphics::Renderer::GRID_CELL_SIZE_PX,
		Graphics::Renderer::GRID_CELL_SIZE_PX
	);

	const std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

	Graphics::Sprite sprite = renderer->create_sprite_from_surface(Graphics::SpriteType::TILE_DEFAULT);
	
	std::shared_ptr<WorldTile> tile = std::make_shared<WorldTile>(scale, position, sprite);
	return tile;
}

void WorldTile::on_left_click()
{
	const Utilities::ivec2 worldPos
	{
		static_cast<int32_t>(get_pos().x),
		static_cast<int32_t>(get_pos().y)
	};

	DEVIOUS_LOG("Clicked tile: " << worldPos.x << ", " << worldPos.y);

	Packets::s_PlayerMovement pos;
	pos.interpreter = PACKET_MOVE_PLAYER;
	pos.x = worldPos.x;
	pos.y = worldPos.y;
	pos.isRunning = true;

	auto packetHandler = g_globals.packetHandler.lock();
	packetHandler->send_packet<Packets::s_PlayerMovement>(&pos, 0, 0);
}

void WorldTile::on_hover()
{
}

void WorldTile::on_stop_hover()
{
}