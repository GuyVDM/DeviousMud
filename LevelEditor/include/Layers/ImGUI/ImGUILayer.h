#pragma once

#include "Layers/Layer.h"

struct SDL_Window;

class ImGUILayer : public Layer 
{
public:
	IMPLEMENT_LAYER_FUNCTIONS;

public:
	ImGUILayer();
	virtual ~ImGUILayer();

private:
	void DrawImGUI();

	void DrawMenuBar();
};