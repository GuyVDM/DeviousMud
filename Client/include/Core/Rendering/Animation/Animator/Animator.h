#pragma once
#include <unordered_map>

#include "Core/Rendering/Sprite/Sprite.h"

#include "Shared/Game/Animations.h"

namespace Graphics
{
	namespace Animation 
	{
		class Animator
		{
			struct AnimationController
			{
				DM::Utils::UUID uuid;
				Sprite*                  sprite = nullptr;
				bool                     bPaused = false;
				e_AnimationType          currentAnimation = e_AnimationType::NO_ANIMATION;
				bool					 bIsLooping = false;
				float					 playbackSpeed = 1.0f;
				int32_t				     keyframeIndex = -1;
				float					 elapsedTime = 0.0f;
				bool                     bHoldOnLastFrame = false;

				e_AnimationType          defaultAnimation     = e_AnimationType::NO_ANIMATION;
				float                    defaultPlaybackspeed = 1.0f;

				void to_default() 
				{
					if (defaultAnimation != e_AnimationType::NO_ANIMATION)
					{
						keyframeIndex    = 0;
						elapsedTime      = 0.0f;
						bPaused          = false;
						bIsLooping       = true;
						currentAnimation = defaultAnimation;
						playbackSpeed    = defaultPlaybackspeed;
						bHoldOnLastFrame = false;
						return;
					}

					DEVIOUS_ERR("Couldn't transition to default animation due to no default being specified.");
				}
			};

		/// <summary>
		/// Static methods
		/// </summary>
		public:
			/// <summary>
			/// Returns true if the sprite is currently playing the specified animation.
			/// </summary>
			/// <param name="_sprite"></param>
			/// <returns></returns>
			static bool is_playing(Sprite& _sprite, e_AnimationType _animation);

			/// <summary>
			/// Register an animation for a given sprite to play.
			/// </summary>
			/// <param name="_sprite"></param>
			/// <param name="_animation"></param>
			/// <param name="_bIsLooping"></param>
			/// <param name="_playbackSpeed"></param>
			static bool play_animation(Sprite& _sprite, const e_AnimationType& _animation, const bool& _bIsLooping, const float& _playbackSpeed = 1.0f);

			/// <summary>
			/// Plays an animation, but holds it at the last frame until a different animation plays.
			/// </summary>
			/// <param name="_sprite"></param>
			/// <param name="_animation"></param>
			/// <param name="_playbackSpeed"></param>
			static void play_animation_oneshot(Sprite& _sprite, const e_AnimationType& _animation, const float& _playbackSpeed = 1.0f);

			/// <summary>
			/// Sets an idle animation for the sprite, everytime a animation comes to end, it will transition back to this specified animation.
			/// This animation will loop by default.
			/// </summary>
			static void set_default_animation(Sprite& _sprite, const e_AnimationType& _animation, const float& _playbackSpeed = 1.0f);

			/// <summary>
			/// Reverts to default animation or pauses on the last rendered frame.
			/// </summary>
			/// <param name="_sprite"></param>
			static void stop_current_animation(Sprite& _sprite);

			/// <summary>
			/// Update all registered animations, and update all animations accordingly.
			/// </summary>
			static void update();

			/// <summary>
			/// Destroy all animators.
			/// </summary>
			static void clear();

		private:
			static void destroy(const uint64_t _uuid);

			static std::unordered_map<DM::Utils::UUID, AnimationController> s_animators;
		};
	}
}