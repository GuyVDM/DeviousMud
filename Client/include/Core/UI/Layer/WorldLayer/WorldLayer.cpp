#include "precomp.h"

#include "Core/UI/Layer/WorldLayer/WorldLayer.h"

#include "Core/Events/Clickable/Tile/WorldTile.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

void Graphics::UI::WorldLayer::init()
{
	//TODO: Data should later get loaded in chunks, size & stuff should get rendered relative to the camera.
	const std::shared_ptr<Graphics::Renderer>& m_renderer = g_globals.m_renderer.lock();

	const Utilities::vec2 WORLD_SIZE{ 10, 10 };

	for (int32_t i = 0; i < WORLD_SIZE.x; i++)
	{
		for(int32_t j = 0; j < WORLD_SIZE.y; j++)
		{
			m_tiles.push_back(WorldTile::create_tile
			(
				Utilities::vec2
				(
					static_cast<float>(i), 
					static_cast<float>(j)
				)
			));
		}
	}
}

void Graphics::UI::WorldLayer::update()
{
	for(const auto& tile : m_tiles) 
	{
		m_renderer->plot_frame(tile->get_sprite(), tile->get_position(), tile->get_size());
	}
}

bool Graphics::UI::WorldLayer::handle_event(const SDL_Event* m_event)
{
	for(const auto& tile : m_tiles) 
	{
		if(tile->handle_event(m_event)) 
		{
			return true;
		}
	}

	return false;
}
