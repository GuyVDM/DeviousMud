#include "precomp.h"

#include "Core/Editor/Editor.h"

#include "Core/Config/Config.h"
#include "Core/Config/Time/Time.hpp"
#include "Core/Camera/Camera.h"
#include "Core/Globals/Globals.h"
#include "Core/Layers/LayerStack.h"
#include "Core/Layers/ImGUI/ImGUILayer.h"
#include "Core/Input/InputHandler/InputHandler.h"
#include "Core/Layers/EditorLayer/EditorLayer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"

#include "Shared/Game/SpriteTypes.hpp"

I32 Editor::s_WindowWidth  = 1;
I32 Editor::s_WindowHeight = 1;

bool Editor::CreateEditor(const I32& _width, const I32& _height)
{
	const Utilities::ivec2 windowSize = Utilities::ivec2(_width, _height);

	//*-----------------
	// Create globals
	//
	{
		g_globals.Renderer    = std::make_shared<Renderer>(windowSize);
		g_globals.Camera      = std::make_shared<Camera>();
		g_globals.WorldEditor = std::make_shared<WorldEditor>();
	}

	SDL_GetWindowSize(g_globals.Renderer->GetWindow(), &s_WindowWidth, &s_WindowHeight);

	m_Renderer     = g_globals.Renderer;
	m_InputHandler = std::make_shared<InputHandler>();

	return true;
}

void Editor::Start()
{
	GenerateLayers();

	while (m_bIsRunning)
	{
		Time::Update();

		m_Renderer->StartFrame();

		m_InputHandler->Update();

		HandleEvents();
		
		m_Renderer->EndFrame();
	}
}

void Editor::GenerateLayers()
{
	m_LayerStack = std::make_shared<LayerStack>();

	m_LayerStack->PushBackLayer(std::make_shared<EditorLayer>());
	m_LayerStack->PushBackLayer(std::make_shared<ImGUILayer>(g_globals.Renderer->GetWindow(), g_globals.Renderer->GetRenderer()));
}

void Editor::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_WINDOWEVENT:
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				s_WindowWidth = event.window.data1;
				s_WindowHeight = event.window.data2;
			}
		}
		break;

		case SDL_QUIT:
		{
			m_bIsRunning = false;
		}
		return;
	}

	//*-------------------
	// Extract all input data.
	//
	{
		m_InputHandler->ExtractInput(event);
	}

	m_LayerStack->Update();
	m_LayerStack->HandleEvent(event);
}
