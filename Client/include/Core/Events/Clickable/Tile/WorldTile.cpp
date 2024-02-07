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
	const Utilities::vec2 pos = 
	{ 
		position.x * Graphics::Renderer::GRID_CELL_SIZE_PX,
		position.y * Graphics::Renderer::GRID_CELL_SIZE_PX
	};

	const Utilities::vec2 scale
	(
		Graphics::Renderer::GRID_CELL_SIZE_PX,
		Graphics::Renderer::GRID_CELL_SIZE_PX
	);

	const std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

	Graphics::Sprite sprite = renderer->create_sprite_from_surface(Graphics::SpriteType::TILE_DEFAULT);
	
	std::shared_ptr<WorldTile> tile = std::make_shared<WorldTile>(scale, pos, sprite);
	return tile;
}

const Utilities::ivec2& WorldTile::get_tile_position() const
{
	Utilities::ivec2 worldPos(0);

	if (pos.x != 0)
	{
		worldPos.x = static_cast<int32_t>(pos.x / Graphics::Renderer::GRID_CELL_SIZE_PX);
	}

	if (pos.y != 0)
	{
		worldPos.y = static_cast<int32_t>(pos.y / Graphics::Renderer::GRID_CELL_SIZE_PX);
	}

	return worldPos;
}

const Utilities::vec2& WorldTile::get_center_position() const
{
	const float half_extends = Graphics::Renderer::GRID_CELL_SIZE_PX * 0.5f;

	return
	{
		pos.x + half_extends,
		pos.y + half_extends
	};
}

void WorldTile::on_left_click()
{
	const Utilities::ivec2 worldPos = get_tile_position();

	std::cout << "Clicked tile: " << worldPos.x << ", " << worldPos.y << std::endl;

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