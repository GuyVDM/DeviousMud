#include "precomp.h"
#include "Core/UI/Layer/Layer.h"

#include "Core/Application/Application.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Events/Clickable/Clickable.h"

#include "Core/Global/C_Globals.h"

Graphics::UI::Layer::Layer()
{
	m_renderer = g_globals.renderer.lock();
}

void Graphics::UI::Layer::init()
{
	return;
}

//TODO: Generalise the way layers behave, currently they all depend on 'Clickable's.
void Graphics::UI::Layer::update()
{
}

bool Graphics::UI::Layer::handle_event(const SDL_Event* _event)
{
	return false;
}
