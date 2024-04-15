#include "precomp.h"

#include "Core/Events/Clickable/Tile/WorldTile.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

WorldTile::~WorldTile()
{
}

std::shared_ptr<WorldTile> WorldTile::create_tile(const Utilities::vec2& position)
{
	//Scale each tile up to the specified cellsize within a grid.
	const Utilities::vec2 scale
	(
		Graphics::Renderer::GRID_CELL_PX_SIZE,
		Graphics::Renderer::GRID_CELL_PX_SIZE
	);

	const std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();
	const Graphics::Sprite sprite = renderer->get_sprite(Graphics::SpriteType::TILE_DEFAULT);

	std::shared_ptr<WorldTile> tile = std::make_shared<WorldTile>(position, scale, sprite);
	return tile;
}

void WorldTile::on_left_click()
{
	const Utilities::ivec2 worldPos
	{
		static_cast<int32_t>(get_position().x),
		static_cast<int32_t>(get_position().y)
	};

	Packets::s_PlayerMovement pos;
	pos.interpreter = PACKET_MOVE_PLAYER;
	pos.x = worldPos.x;
	pos.y = worldPos.y;
	pos.isRunning = true;

	auto packetHandler = g_globals.packetHandler.lock();
	packetHandler->send_packet<Packets::s_PlayerMovement>(&pos, 0, 0);
}

bool WorldTile::handle_event(const SDL_Event* _event)
{
	if (is_hovered())
	{
		switch (_event->type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				if (_event->button.button == SDL_BUTTON_RIGHT)
				{
					OptionArgs args;
					args.actionStr = "Walk hereaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
					args.subjectStr = "";
					args.function = std::bind(&WorldTile::on_left_click, this);

					OptionsTab::add_option(args);
				}
			}
			break;
		}
	}

	return Clickable::handle_event(_event);
}

void WorldTile::on_hover()
{
}

void WorldTile::on_hover_end()
{
}