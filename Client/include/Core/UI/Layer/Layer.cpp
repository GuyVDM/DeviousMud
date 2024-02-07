#include "precomp.h"
#include "Layer.h"

#include "Core/Application/Application.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Events/Clickable/Clickable.h"

#include "Core/Global/C_Globals.h"

Graphics::UI::Layer::Layer()
{
	renderer = g_globals.renderer;
}

void Graphics::UI::Layer::init()
{
	return;
}

//TODO: Generalise the way layers behave, currently they all depend on 'Clickable's.
void Graphics::UI::Layer::update()
{
	for (const auto& eventReceiver : receivers)
	{
		const std::shared_ptr<Renderer> r = renderer.lock();
		const std::shared_ptr<Clickable> clickable = std::static_pointer_cast<Clickable>(eventReceiver);
		r->plot_frame(clickable->get_sprite(), clickable->get_pos(), clickable->get_size());
	}
}

bool Graphics::UI::Layer::handle_event(const SDL_Event* event)
{
	for(const auto& receiver : receivers) 
	{
		if(receiver ->handle_event(event)) 
		{
			return true;
		}
	}

	return false;
}

void Graphics::UI::Layer::subscribe_to_event(EventReceiver* receiver)
{
	receivers.push_back
	(
		std::shared_ptr<EventReceiver>(receiver)
	);
}
