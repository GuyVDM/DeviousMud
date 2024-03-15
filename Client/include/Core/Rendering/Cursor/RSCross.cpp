#include "precomp.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Rendering/Cursor/RSCross.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/Global/C_Globals.h"

using namespace Graphics;
using namespace Animation;

RSCross::RSCross(Utilities::vec2 _scale) : scale(_scale)
{
	renderer = g_globals.renderer.lock();
	sprite = renderer->get_sprite(SpriteType::CROSS);
	sprite.frame = 12;
}

void RSCross::click(Utilities::vec2 _position, e_InteractionType _interactionType)
{
	const e_AnimationType animation = (_interactionType == e_InteractionType::RED_CLICK) ? e_AnimationType::ANIMATION_CLICK_RED 
																					     : e_AnimationType::ANIMATION_CLICK_YELLOW;
	// Center the middle of the mouse to the middle of the sprite.
	{
		const Utilities::vec2 offset = scale / 2.0f;
		position = _position - offset;
	}

	Animator::play_animation(sprite, animation, false, 24.0f);
}

void RSCross::update()
{
	renderer->plot_raw_frame(sprite, position, scale);
}
