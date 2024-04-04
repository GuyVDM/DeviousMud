#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/UI/UIComponent/Canvas/Canvas.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/UIComponent/HUDTabMenu/HUDTabMenu.h"

using namespace Utilities;
using namespace Graphics;
using namespace UI;

e_UIInteractionType HUDLayer::sInteractionType = e_UIInteractionType::INTERACT;

e_UIInteractionType HUDLayer::get_interaction_type()
{
	return sInteractionType;
}

void HUDLayer::init()
{
	create_hud();
}

bool HUDLayer::handle_event(const SDL_Event* event)
{
	//TODO: MOVE THIS DISGUSTING SHIT TO A INPUT CLASS AT SOME POINT.
	static bool altDown = false;
	static bool leftMouseDown = false;

	switch (event->type)
	{
		case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_LALT || event->key.keysym.sym == SDLK_RALT)
		{
			altDown = true;
		}
		break;

		case SDL_MOUSEBUTTONDOWN:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			leftMouseDown = event->button.button == SDL_BUTTON_LEFT;
		}
		break;

		case SDL_MOUSEBUTTONUP:
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			leftMouseDown = false;
		}
		break;

		case SDL_KEYUP:
		if (event->key.keysym.sym == SDLK_LALT || event->key.keysym.sym == SDLK_RALT)
		{
			altDown = false;
		}
		break;

		case SDL_MOUSEMOTION:
		{
			if(UIComponent::sDraggedComponent) 
			{
				Utilities::ivec2 mousePos;
				SDL_GetMouseState(&mousePos.x, &mousePos.y);

				UIComponent* draggedComponent = UIComponent::sDraggedComponent;
				draggedComponent->set_position(Utilities::to_vec2(mousePos) - UIComponent::sDragOffset);
			}
		}
		break;
	}

	/// Set the interaction type to moving if both buttons are pressed.
	/// Moving state makes it so that any hovered on UI elements follows the mouse.
	sInteractionType = altDown && leftMouseDown ? e_UIInteractionType::MOVE    :
		                                altDown ? e_UIInteractionType::DISPLAY :
		                                          e_UIInteractionType::INTERACT;

	return canvas->handle_event(event);
}

void HUDLayer::create_hud()
{
	canvas = Canvas::create_canvas();

	std::shared_ptr<UIComponent> component;
	
	// HUD Tab Menu
	{
		Utilities::ivec2 viewportSize = Utilities::ivec2(0); 
		renderer->get_viewport_size(&viewportSize.x, &viewportSize.y);

		component = UIComponent::create_component<HUDTabMenu>
		(
			UIComponent::Position(0.0f),
			UIComponent::Size(336.0f * 0.625f, 504.0f * 0.625f),
			SpriteType::HUD_BACKDROP,
			true
		);

		const Rect boundingRect = component->get_bounding_rect();
		const Utilities::vec2 size = boundingRect.get_size();
		const Utilities::vec2 offset = component->get_position() - Utilities::vec2(boundingRect.minPos.x, boundingRect.minPos.y);
		const Utilities::vec2 position = Utilities::to_vec2(viewportSize) - size + offset;
		component->set_position(position);
		component->set_anchor(e_AnchorPreset::BOTTOM_RIGHT);
		canvas->add_child(component);
	}
}

void HUDLayer::update()
{
	std::shared_ptr<UIComponent> base = std::dynamic_pointer_cast<UIComponent>(canvas);

	// Render loop
	{	
		base->render(renderer);
	}
}

