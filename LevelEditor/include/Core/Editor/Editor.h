#pragma once
#include "Core/Core.h"

struct SDL_Renderer;
struct SDL_Window;

class LayerStack;

class Renderer;
class InputHandler;

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
	bool CreateEditor(const I32& _width = 0, const I32& _height = 0);

	void Start();

public:
	Editor() = default;
	virtual ~Editor();

private:
	void GenerateLayers();

	void HandleEvents();

private:
	bool m_bIsRunning = true;

	Ref<LayerStack> m_LayerStack;

	Ref<Renderer>     m_Renderer;

	Ref<InputHandler> m_InputHandler;
};