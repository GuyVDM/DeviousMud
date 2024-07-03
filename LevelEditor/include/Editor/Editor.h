#pragma once
#include "Core/Core.hpp"

struct SDL_Renderer;
struct SDL_Window;

class LayerStack;

class Renderer;

class Editor 
{
public:
	static U32 s_WindowWidth;

	static U32 s_WindowHeight;

	/// <summary>
	/// Create window with specified sizes.
	/// For fullscreen don't specify any.
	/// </summary>
	/// <param name="_width"></param>
	/// <param name="_height"></param>
	bool CreateEditorWindow(int _width = 0, int _height = 0);

	void Start();

public:
	Editor() = default;
	virtual ~Editor();

private:
	void GenerateLayers();

private:
	SDL_Window*   m_Window;

	SDL_Renderer* m_SDLRenderer;

	Ref<LayerStack> m_LayerStack;

	Ref<Renderer> m_Renderer;
};