#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/UI/UIComponent/Canvas/Canvas.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/UIComponent/HUDTabMenu/HUDTabMenu.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

using namespace Utilities;
using namespace Graphics::UI;

e_UIInteractionType HUDLayer::s_InteractionType = e_UIInteractionType::INTERACT;

e_UIInteractionType HUDLayer::get_interaction_type()
{
	return s_InteractionType;
}

void HUDLayer::init()
{
	create_hud();
}

bool HUDLayer::handle_event(const SDL_Event* _event)
{
	//TODO: MOVE THIS DISGUSTING STUFF TO A INPUT CLASS AT SOME POINT.
	static bool altDown = false;
	static bool leftMouseDown = false;
	static bool shouldOpenoptionsMenu = false;

	switch (_event->type)
	{
		case SDL_KEYDOWN:
		if (_event->key.keysym.sym == SDLK_LALT || _event->key.keysym.sym == SDLK_RALT)
		{
			altDown = true;
		}
		break;

		case SDL_MOUSEBUTTONDOWN:
		{
			if (_event->button.button == SDL_BUTTON_LEFT)
			{
				leftMouseDown = _event->button.button == SDL_BUTTON_LEFT;
			}
		}
		break;

		case SDL_MOUSEBUTTONUP:
		if (_event->button.button == SDL_BUTTON_LEFT)
		{
			leftMouseDown = false;
		}
		break;

		case SDL_KEYUP:
		if (_event->key.keysym.sym == SDLK_LALT || _event->key.keysym.sym == SDLK_RALT)
		{
			altDown = false;
		}
		break;

		case SDL_MOUSEMOTION:
		{
			if(UIComponent::s_draggedComponent) 
			{
				Utilities::ivec2 mousePos;
				SDL_GetMouseState(&mousePos.x, &mousePos.y);

				UIComponent* draggedComponent = UIComponent::s_draggedComponent;
				draggedComponent->set_position(Utilities::to_vec2(mousePos) - UIComponent::s_dragOffset);
			}
		}
		break;
	}

	/// Set the interaction type to moving if both buttons are pressed.
	/// Moving state makes it so that any hovered on UI elements follows the mouse.
	s_InteractionType = altDown && leftMouseDown ? e_UIInteractionType::MOVE    :
		                                altDown ? e_UIInteractionType::DISPLAY :
		                                          e_UIInteractionType::INTERACT;

	return m_canvas->handle_event(_event);
}

void HUDLayer::create_hud()
{
	m_canvas = Canvas::create_canvas();

	std::shared_ptr<UIComponent> component;

	// HUD Tab Menu
	{
		Utilities::ivec2 viewportSize = Utilities::ivec2(0); 
		m_renderer->get_viewport_size(&viewportSize.x, &viewportSize.y);

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
		const Utilities::vec2 m_position = Utilities::to_vec2(viewportSize) - size + offset;
		component->set_position(m_position);
		component->set_anchor(e_AnchorPreset::BOTTOM_RIGHT);
		m_canvas->add_child(component);
	}

	//// Options Menu
	{
		component = UIComponent::create_component<OptionsTab>
			(
				UIComponent::Position(100.0f, 100.0f),
				UIComponent::Size(100.0f, 50.0f),
				SpriteType::HUD_OPTIONS_BOX,
				true
			);
	}   m_canvas->add_child(component);
}

void HUDLayer::update()
{
	std::shared_ptr<UIComponent> base = std::dynamic_pointer_cast<UIComponent>(m_canvas);

	// Render loop
	{	
		base->render(m_renderer);
	}
}

Graphics::UI::HUDLayer::~HUDLayer()
{
	UIComponent::s_draggedComponent = nullptr;
}

