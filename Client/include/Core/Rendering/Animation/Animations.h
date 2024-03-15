#pragma once
#include <unordered_map>

#include "Core/Rendering/Animation/Animation2D.h"

enum class e_AnimationType : uint8_t
{
	ANIMATION_CLICK_YELLOW = 0x00,
	ANIMATION_CLICK_RED    = 0x01
};

struct Animations
{
    using AnimationMap = std::unordered_map<e_AnimationType, Animation2D>;

    static const AnimationMap animationMap()
    {
        static const AnimationMap animations = 
        {
             { e_AnimationType::ANIMATION_CLICK_YELLOW,  Animation2D({0 , 1 , 2 , 3 , 4 ,  5, 12 })},
             { e_AnimationType::ANIMATION_CLICK_RED,     Animation2D({6 , 7 , 8 , 9 , 10, 11, 12 })}
        };

        return animations;
    }
};