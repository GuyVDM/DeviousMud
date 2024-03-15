#pragma once
#include <unordered_map>

#include "Core/Rendering/Sprite/Sprite.h"

#include "Core/Rendering/Animation/Animations.h"

namespace Graphics
{
	namespace Animation 
	{
		class Animator
		{
			struct AnimationController
			{
				DEVIOUSMUD::RANDOM::UUID uuid;
				Sprite*                  sprite;
				e_AnimationType          currentAnimation;
				bool					 bIsLooping;
				float					 playbackSpeed;
				int32_t				     keyframeIndex;
				float					 elapsedTime;
			};

		/// <summary>
		/// Static methods
		/// </summary>
		public:
			/// <summary>
			/// Register an animation for a given sprite to play.
			/// </summary>
			/// <param name="_sprite"></param>
			/// <param name="_animation"></param>
			/// <param name="_bIsLooping"></param>
			/// <param name="_playbackSpeed"></param>
			static void play_animation(Sprite& _sprite, const e_AnimationType& _animation, const bool& _bIsLooping, const float& _playbackSpeed = 1.0f);

			/// <summary>
			/// Update all registered animations, and update all animations accordingly.
			/// </summary>
			static void update();

			/// <summary>
			/// Destroy all animators.
			/// </summary>
			static void clear();

		private:
			static std::unordered_map<DEVIOUSMUD::RANDOM::UUID, AnimationController> animators;
		};
	}
}