#include "precomp.h"

#include "Core/Layers/LayerStack.h"

#include "Core/Layers/Layer.h"

void LayerStack::PushBackLayer(Ref<Layer> _layer)
{
	m_Layers.push_back(_layer);
}

void LayerStack::PopLayer()
{
	if(m_Layers.size() > 0)
	{
		m_Layers.pop_back();
	}
}

void LayerStack::Clear()
{
	m_Layers.clear();
}

void LayerStack::HandleEvent(const SDL_Event& _event)
{
	for (auto rit = m_Layers.rbegin(); rit != m_Layers.rend(); ++rit)
	{
		const U64 i = std::distance(rit, m_Layers.rend()) - 1;
		
		if(m_Layers[i]->HandleEvent(_event)) 
		{
			return;
		}
	}
}

void LayerStack::Update()
{
	for (auto rit = m_Layers.rbegin(); rit != m_Layers.rend(); ++rit) 
	{
		const U64 i = std::distance(rit, m_Layers.rend()) - 1;
		m_Layers[i]->Update();
	}
}
