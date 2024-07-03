#pragma once
#include "Core/Core.hpp"

#include "Layers/Layer.h"

#include <vector>

typedef union SDL_Event SDL_Event;

class LayerStack 
{
public:
	void PushBackLayer(Ref<Layer> _layer);

	void PopLayer();

	void Clear();

	void HandleEvent(const SDL_Event& _event);

	void Update();

private:
	std::vector<Ref<Layer>> m_Layers;
};