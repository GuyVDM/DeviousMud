#pragma once
#include <unordered_map>

#include "Core/Rendering/Animation/Animation2D.h"

enum class e_AnimationType : uint8_t
{
    NO_ANIMATION           = 0x00,
	ANIMATION_CLICK_YELLOW = 0x01,
	ANIMATION_CLICK_RED    = 0x02,
    PLAYER_IDLE            = 0x03,
    PLAYER_WALKING         = 0x04
};

struct Animations
{
    using AnimationMap = std::unordered_map<e_AnimationType, Animation2D>;

    static const AnimationMap animationMap()
    {
        static const AnimationMap animations = 
        {
             { e_AnimationType::ANIMATION_CLICK_YELLOW,  Animation2D({0 , 1 , 2 , 3 , 4 ,  5, 12 })},
             { e_AnimationType::ANIMATION_CLICK_RED,     Animation2D({6 , 7 , 8 , 9 , 10, 11, 12 })},
             { e_AnimationType::PLAYER_IDLE,             Animation2D({0 , 1 , 2 , 3 , 4 })},
             { e_AnimationType::PLAYER_WALKING,          Animation2D({5 , 6 , 7 , 8 , 9, 10, 11, 12 })}

        };

        return animations;
    }
};