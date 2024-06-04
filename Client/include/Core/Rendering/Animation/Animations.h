#pragma once
#include <unordered_map>

#include "Core/Rendering/Animation/Animation2D.h"

enum class e_AnimationType : uint8_t
{
    NO_ANIMATION = 0x00,

    //Clicking cross sprite
	ANIMATION_CLICK_YELLOW,
	ANIMATION_CLICK_RED,

    //Player
    PLAYER_IDLE,
    PLAYER_WALKING,
    PLAYER_ATTACKING,
    PLAYER_DEATH,

    //Entities
    GOBLIN_IDLE,
    GOBLIN_WALK,
    GOBLIN_ATTACK,
    GOBLIN_DEATH
};

struct Animations
{
    using AnimationMap = std::unordered_map<e_AnimationType, Animation2D>;

    static const AnimationMap animationMap()
    {
        static const AnimationMap animations = 
        {
             { e_AnimationType::ANIMATION_CLICK_YELLOW,  Animation2D(0 , 6 )},
             { e_AnimationType::ANIMATION_CLICK_RED,     Animation2D(7 , 13)},
             { e_AnimationType::PLAYER_IDLE,             Animation2D(0 , 4 )},
             { e_AnimationType::PLAYER_WALKING,          Animation2D(5 , 12)},
             { e_AnimationType::PLAYER_ATTACKING,        Animation2D(17 , 20)},
             { e_AnimationType::PLAYER_DEATH,            Animation2D(21 , 24)},

             //Entities
             { e_AnimationType::GOBLIN_IDLE,             Animation2D(0 , 19)},
             { e_AnimationType::GOBLIN_WALK,             Animation2D(20, 31)},
             { e_AnimationType::GOBLIN_ATTACK,           Animation2D(32, 39)},
             { e_AnimationType::GOBLIN_DEATH,            Animation2D(40, 49)}

        };

        return animations;
    }
};