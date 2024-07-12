#pragma once

#include "Core/Layers/Layer.h"

#include "Core/Core.hpp"

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