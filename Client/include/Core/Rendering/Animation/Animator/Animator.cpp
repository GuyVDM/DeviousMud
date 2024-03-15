#include "precomp.h"

#include "Shared/Utilities/Math.hpp"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/Application/Config/Config.h"

using namespace Graphics;
using namespace Animation;

using namespace DEVIOUSMUD;
using namespace RANDOM;

// All registered controllers.
std::unordered_map<DEVIOUSMUD::RANDOM::UUID, Animator::AnimationController> Animator::animators;

void Animator::play_animation(Sprite& _sprite, const e_AnimationType& _animationType, const bool& _bIsLooping, const float& _playbackSpeed)
{
	AnimationController controller;
	controller.uuid             = _sprite.get_uuid();
	controller.sprite           = &_sprite;
	controller.currentAnimation = _animationType;
	controller.bIsLooping       = _bIsLooping;
	controller.playbackSpeed    = _playbackSpeed;
	controller.keyframeIndex    = 0;
	controller.elapsedTime      = 0.0f;

	//Register the animation controller
	Animator::animators[controller.uuid] = controller;

	////Set the first frame of the animation
	const Animation2D animation = Animations::animationMap().at(_animationType);
	controller.sprite->frame = animation.get_keyframes()[controller.keyframeIndex];
}

void Animator::update()
{
	const float deltaTime = DEVIOUSMUD::CLIENT::Config::get_deltaTime();

	std::vector<UUID> toDestroy;

	for (auto& pair : animators)
	{
		AnimationController& controller = pair.second;
		controller.elapsedTime += (deltaTime * controller.playbackSpeed);

		if(controller.elapsedTime > 1.0f) 
		{
			//Check if the sprite reference still exists, if not, mark this animator for delete and continue.
			if (controller.sprite == nullptr)
			{
				Destroy:
					toDestroy.push_back(controller.uuid);
					continue;
			}

			controller.elapsedTime = 0.0f;
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
					goto Destroy;
				}
			}

			// Increment keyframe index and set new animation frame.
			{
				controller.sprite->frame = animation.get_keyframes()[controller.keyframeIndex];
			}
		}
	}

	// Destroy any animators marked for delete.
	{
		for(UUID& uuid : toDestroy) 
		{
			Animator::animators.erase(uuid);
		}
	}
}

void Animator::clear()
{
	Animator::animators.clear();
}
