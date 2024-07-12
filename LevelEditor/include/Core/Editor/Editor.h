#pragma once
#include "Core/Core.hpp"

struct SDL_Renderer;
struct SDL_Window;

class LayerStack;

class Renderer;

class Editor 
{
public:
	static I32 s_WindowWidth;

	static I32 s_WindowHeight;

	/// <summary>
	/// Create window with specified sizes.
	/// For fullscreen don't specify any.
	/// </summary>
	/// <param name="_width"></param>
	/// <param name="_height"></param>
	bool CreateEditorWindow(const I32& _width = 0, const I32& _height = 0);

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