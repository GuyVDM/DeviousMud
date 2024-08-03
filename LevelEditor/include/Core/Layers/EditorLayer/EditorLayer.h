#pragma once

#include "Core/Layers/Layer.h"

#include "Core/Core.h"

class EditorLayer : public Layer
{
public:
	IMPLEMENT_LAYER_FUNCTIONS;

public:
	EditorLayer() = default;
	virtual ~EditorLayer() = default;
};