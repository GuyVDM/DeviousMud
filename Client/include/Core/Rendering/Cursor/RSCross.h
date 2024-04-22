#pragma once
#include "Shared/Utilities/vec2.hpp"

#include "Core/Rendering/Sprite/Sprite.h"

namespace Graphics
{
	class Renderer;
}

enum class e_InteractionType
{
	YELLOW_CLICK = 0,
	RED_CLICK = 1
};

class RSCross 
{
public:
	RSCross(Utilities::vec2 _scale);
	~RSCross() = default;

	void click(Utilities::vec2 _position, e_InteractionType _interactionType);

	void update();

private:
	Utilities::vec2  m_position;
	Utilities::vec2  m_scale;

	Graphics::Sprite m_sprite;

	std::shared_ptr<Graphics::Renderer> m_renderer;
};