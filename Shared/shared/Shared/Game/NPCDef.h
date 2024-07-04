#pragma once
#include "Shared/Game/SpriteTypes.hpp"

#include "Shared/Game/Animations.h"

#include <map>

#include <string>

namespace Graphics
{
	enum class SpriteType : uint16_t;
}

struct NPCDef 
{
	std::string          name           = "null";
	uint32_t             combatLevel    = 0;
	uint8_t              size           = 1;                             //UNUSED
	Graphics::SpriteType sprite;
	e_AnimationType      walkingAnim    = e_AnimationType::NO_ANIMATION;
	e_AnimationType      attackAnim     = e_AnimationType::NO_ANIMATION;
	e_AnimationType      idleAnim       = e_AnimationType::NO_ANIMATION;
	e_AnimationType      deathAnim		= e_AnimationType::NO_ANIMATION;
};

using NPCMap = std::map<uint8_t, NPCDef>;

inline const static NPCDef get_npc_definition(uint8_t _id)
{
	using namespace Graphics;

	NPCDef npc;

	switch(_id) 
	{
	case 0:
		npc.name = "Player";
		npc.combatLevel = 0;
		npc.sprite      = SpriteType::PLAYER;
		npc.size = 1;
		/// THESE ANIMATIONS NEEDS TO GET GRABBED FROM THE PLAYER HANDLER IN THE FUTURE.
		///
		/// 
		npc.idleAnim    = e_AnimationType::PLAYER_IDLE;
		npc.walkingAnim = e_AnimationType::PLAYER_WALKING;
		npc.attackAnim  = e_AnimationType::PLAYER_ATTACKING;
		npc.deathAnim   = e_AnimationType::PLAYER_DEATH;
		break;

	case 1:
		npc.name = "Goblin";
		npc.combatLevel = 3;
		npc.sprite      = SpriteType::NPC_GOBLIN;
		npc.size = 1;
		npc.idleAnim    = e_AnimationType::GOBLIN_IDLE;
		npc.walkingAnim = e_AnimationType::GOBLIN_WALK;
		npc.attackAnim  = e_AnimationType::GOBLIN_ATTACK;
		npc.deathAnim   = e_AnimationType::GOBLIN_DEATH;
		break;

	case 2:
		npc.name = "Inu";
		npc.combatLevel = 3;
		npc.size = 1;
		npc.sprite      = SpriteType::NPC_INU;
		npc.idleAnim    = e_AnimationType::INU_IDLE;
		npc.walkingAnim = e_AnimationType::INU_WALK;
		npc.attackAnim  = e_AnimationType::INU_ATTACK;
		npc.deathAnim   = e_AnimationType::INU_DEATH;
		break;

	default:
		npc.name = "null";
		npc.combatLevel = 0;
		npc.sprite      = SpriteType::NONE;
		npc.size = 1;
		npc.idleAnim    = e_AnimationType::NO_ANIMATION;
		npc.walkingAnim = e_AnimationType::NO_ANIMATION;
		npc.attackAnim  = e_AnimationType::NO_ANIMATION;
		npc.deathAnim   = e_AnimationType::NO_ANIMATION;
		break;
	}

	return npc;
}