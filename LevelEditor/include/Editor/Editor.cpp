#include "precomp.h"

#include "Editor/Editor.h"

#include "Layers/LayerStack.h"

#include "Layers/ImGUI/ImGUILayer.h"

#include "Layers/InputLayer/InputLayer.h"

#include "Renderer/Renderer.h"

#include "Config/Config.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "Shared/Game/SpriteTypes.hpp"

Globals g_globals;

U32 Editor::s_WindowWidth  = 1;
U32 Editor::s_WindowHeight = 1;

bool Editor::CreateEditorWindow(int _width, int _height)
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) 
	{
		DEVIOUS_ERR(SDL_GetError());
		return false;
	}

	U32 flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	if(_width == 0 && _height == 0) 
	{
		flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
		m_Window = SDL_CreateWindow("DM Level Editor", 0, 0, 1000, 1000, flags);
	}
	else
	{
		m_Window = SDL_CreateWindow("DM Level Editor", 0, 0, _width, _height, flags);
	}

	SDL_GetWindowSize(m_Window, &s_WindowWidth, &s_WindowHeight);

	if(!m_Window) 
	{
		DEVIOUS_LOG("Window couldn't get created: " << SDL_GetError());
	}

	//*----------------------------
	// Initialise IMGUI
	//*
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
		}

		m_SDLRenderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(m_SDLRenderer, SDL_BLENDMODE_BLEND);

		ImGui_ImplSDL2_InitForSDLRenderer(m_Window, m_SDLRenderer);
		ImGui_ImplSDLRenderer2_Init(m_SDLRenderer);
	}

	//*-----------------
	// Create globals
	//
	{
		g_globals.Renderer = std::make_shared<Renderer>(m_SDLRenderer);
		g_globals.Camera   = std::make_shared<Camera>();
	}

	m_Renderer = g_globals.Renderer;

	return true;
}

void Editor::Start()
{
	GenerateLayers();

	bool bIsRunning = true;
	SDL_Event event;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	while (bIsRunning)
	{
		App::Config::EditorConfig::UpdateDT();

		m_Renderer->StartFrame();

		while (SDL_PollEvent(&event))
		{
			switch (event.type) 
			{
				case SDL_WINDOWEVENT:
				{
					if(event.window.event == SDL_WINDOWEVENT_RESIZED) 
					{
						s_WindowWidth  = event.window.data1;
						s_WindowHeight = event.window.data2;
					}
				}
				break;

				case SDL_QUIT:
				{
					bIsRunning = false;
				}
				break;
			}

			m_LayerStack->HandleEvent(event);
		}

		m_LayerStack->Update();

		m_Renderer->EndFrame();

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_SDLRenderer);

		SDL_RenderPresent(m_SDLRenderer);
	}
}

void Editor::GenerateLayers()
{
	m_LayerStack = std::make_shared<LayerStack>();

	m_LayerStack->PushBackLayer(std::make_shared<InputLayer>());
	m_LayerStack->PushBackLayer(std::make_shared<ImGUILayer>());
}

Editor::~Editor()
{
	SDL_DestroyWindow(m_Window);
	SDL_DestroyRenderer(m_SDLRenderer);

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
