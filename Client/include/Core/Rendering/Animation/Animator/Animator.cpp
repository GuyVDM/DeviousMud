#include "precomp.h"

#include "Shared/Utilities/Math.hpp"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/Application/Config/Config.h"

using namespace Graphics;
using namespace Animation;

using namespace DM;
using namespace CLIENT;
using namespace Utils;

// All registered controllers.
std::unordered_map<DM::Utils::UUID, Animator::AnimationController> Animator::s_animators;

void Animator::play_animation(Sprite& _sprite, const e_AnimationType& _animationType, const bool& _bIsLooping, const float& _playbackSpeed)
{
	////Set the first frame of the animation
	const Animation2D animation = Animations::animationMap().at(_animationType);

	if (Animator::s_animators.find(_sprite.get_uuid()) == Animator::s_animators.end())
	{
		AnimationController controller;
		controller.uuid = _sprite.get_uuid();
		controller.m_sprite = &_sprite;
		controller.bPaused = false;
		controller.currentAnimation = _animationType;
		controller.bIsLooping = _bIsLooping;
		controller.playbackSpeed = _playbackSpeed;
		controller.keyframeIndex = 0;
		controller.m_elapsedTime = 0.0f;

		//Register the animation controller
		Animator::s_animators[controller.uuid] = controller;
		controller.m_sprite->frame = animation.get_keyframes()[controller.keyframeIndex];
		return;
	}

	// If the controller already exists, just update the animation within the controller.
	{
		AnimationController& controller = Animator::s_animators[_sprite.get_uuid()];
		controller.bPaused = false;
		controller.currentAnimation = _animationType;
		controller.bIsLooping = _bIsLooping;
		controller.playbackSpeed = _playbackSpeed;
		controller.keyframeIndex = 0;
		controller.m_elapsedTime = 0.0f;
		controller.m_sprite->frame = animation.get_keyframes()[controller.keyframeIndex];

		Animator::s_animators[controller.uuid] = controller;
	}
}

void Graphics::Animation::Animator::set_default_animation(Sprite& _sprite, const e_AnimationType& _animation, const float& _playbackSpeed)
{
	if (_animation != e_AnimationType::NO_ANIMATION) 
	{
		if (Animator::s_animators.find(_sprite.get_uuid()) == Animator::s_animators.end())
		{
			Animator::play_animation(_sprite, _animation, true, _playbackSpeed);
		}

		AnimationController& controller = Animator::s_animators[_sprite.get_uuid()];
		controller.defaultAnimation = _animation;
		controller.defaultPlaybackspeed = _playbackSpeed;
		return;
	}

	DEVIOUS_WARN("You can't set a default animation as null!");
}

void Graphics::Animation::Animator::stop_current_animation(Sprite& _sprite)
{
	if (Animator::s_animators.find(_sprite.get_uuid()) != Animator::s_animators.end())
	{
		AnimationController& controller = Animator::s_animators[_sprite.get_uuid()];
		
		if(controller.defaultAnimation != e_AnimationType::NO_ANIMATION) 
		{
			controller.to_default();
			return;
		}
		
		controller.bPaused = true;
	}
}

void Animator::update()
{
	for (auto& pair : s_animators)
	{
		AnimationController& controller = pair.second;

		if (!controller.bPaused) 
		{
			controller.m_elapsedTime += Config::get_deltaTime() * controller.playbackSpeed;

			if (controller.m_elapsedTime > 1.0f)
			{
				controller.m_elapsedTime = 0.0f;
				controller.keyframeIndex++;

				//Set the first frame of the animation
				const Animation2D animation = Animations::animationMap().at(controller.currentAnimation);

				// Check if we reached the end of the animation.
				if (controller.keyframeIndex > animation.get_last_frame_index())
				{
					if (controller.bIsLooping)
					{
						controller.keyframeIndex = 0;
					}
					else
					{
						if(controller.defaultAnimation != e_AnimationType::NO_ANIMATION)
						{
							controller.to_default();
						}
						else 
						{
							controller.bPaused = true;
						}
						continue;
					}
				}

				// Set new animation frame.
				{
					controller.m_sprite->frame = animation.get_keyframes()[controller.keyframeIndex];
				}
			}
		}
	}
}

void Animator::clear()
{
	Animator::s_animators.clear();
}

void Graphics::Animation::Animator::destroy(const uint64_t _uuid)
{
	if(Animator::s_animators.find(_uuid) != Animator::s_animators.end()) 
	{
		DEVIOUS_LOG("Destroyed animator of uuid: " << _uuid);
		Animator::s_animators.erase(_uuid);
	}
}
