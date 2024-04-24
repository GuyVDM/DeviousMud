#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/Canvas/Canvas.h"

std::shared_ptr<Canvas> Canvas::create_canvas()
{
	return UIComponent::create_component<Canvas>(Utilities::vec2(0.0f), Utilities::vec2(0.0f), Graphics::SpriteType::NONE);
}

void Canvas::init()
{
	auto renderer = g_globals.renderer.lock();

	// Set the canvas size to match the viewport its dimensions.
	{
		Utilities::ivec2 viewportSize;
		renderer->get_viewport_size(&viewportSize.x, &viewportSize.y);
		set_size(Utilities::to_vec2(viewportSize));
	}

	// Make it so that the canvas always scales to viewport
	renderer->on_viewport_size_changed.add_listener
	(
		std::bind(&Canvas::set_size, this, std::placeholders::_1)
	);
}
