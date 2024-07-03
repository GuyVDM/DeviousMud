#pragma once

#include "Core/Core.hpp"

#include "Layers/Layer.h"

class WorldEditor;

class InputLayer : public Layer
{
public:
	IMPLEMENT_LAYER_FUNCTIONS;

public:
	InputLayer();
	virtual ~InputLayer() = default;

private:
	Ref<WorldEditor> m_WorldEditor;
};