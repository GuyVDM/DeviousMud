#pragma once
#include <unordered_map>

#include "Core/Rendering/Animation/Animation2D.h"

enum class e_AnimationType : uint8_t
{
    NO_ANIMATION           = 0x00,
	ANIMATION_CLICK_YELLOW = 0x01,
	ANIMATION_CLICK_RED    = 0x02,
    PLAYER_IDLE            = 0x03,
    PLAYER_WALKING         = 0x04,

    //Entities
    GOBLIN_IDLE            = 0x05,
    GOBLIN_WALK            = 0x06,
    GOBLIN_ATTACK          = 0x07,
    GOBLIN_DEATH           = 0x08
};

struct Animations
{
    using AnimationMap = std::unordered_map<e_AnimationType, Animation2D>;

    static const AnimationMap animationMap()
    {
        static const AnimationMap animations = 
        {
             { e_AnimationType::ANIMATION_CLICK_YELLOW,  Animation2D(0 , 5 )},
             { e_AnimationType::ANIMATION_CLICK_RED,     Animation2D(6 , 12)},
             { e_AnimationType::PLAYER_IDLE,             Animation2D(0 , 4 )},
             { e_AnimationType::PLAYER_WALKING,          Animation2D(5 , 12)},

             //Entities
             { e_AnimationType::GOBLIN_IDLE,             Animation2D(0 , 19)},
             { e_AnimationType::GOBLIN_WALK,             Animation2D(20, 31)},
             { e_AnimationType::GOBLIN_ATTACK,           Animation2D(32, 39)},
             { e_AnimationType::GOBLIN_DEATH,            Animation2D(40, 49)}

        };

        return animations;
    }
};