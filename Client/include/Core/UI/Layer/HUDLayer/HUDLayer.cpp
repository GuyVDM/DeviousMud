#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/UI/UIComponent/Canvas/Canvas.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/UIComponent/HUDTabMenu/HUDTabMenu.h"

using namespace Utilities;

void Graphics::UI::HUDLayer::init()
{
	create_hud();
}

bool Graphics::UI::HUDLayer::handle_event(const SDL_Event* event)
{
	return canvas->handle_event(event);
}

void Graphics::UI::HUDLayer::create_hud()
{
	canvas = Canvas::create_canvas();

	std::shared_ptr<UIComponent> component;
	
	// HUD Tab Menu
	{
		Utilities::ivec2 viewportSize = Utilities::ivec2(0); 
		renderer->get_viewport_size(&viewportSize.x, &viewportSize.y);

		component = UIComponent::create_component<HUDTabMenu>
		(
			UIComponent::Position(0.0f, 0.0f),
			UIComponent::Size(336.0f, 504.0f),
			SpriteType::HUD_BACKDROP
		);

		Utilities::vec2 position = Utilities::to_vec2(viewportSize) - component->get_size();
		component->set_position(position);
		component->set_anchor(e_AnchorPreset::BOTTOM_RIGHT);
		canvas->add_child(component);
	}
}

void Graphics::UI::HUDLayer::update()
{
	std::shared_ptr<UIComponent> base = std::dynamic_pointer_cast<UIComponent>(canvas);

	// Render loop
	{	
		base->render();
	}
}

