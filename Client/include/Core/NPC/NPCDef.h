#pragma once
#include "Core/Rendering/Sprite/Sprite.h"

#include "Core/Rendering/Animation/Animations.h"

#include <map>

#include <string>

struct NPCDef 
{
	std::string          name;
	uint32_t             combatLevel;
	Graphics::SpriteType m_sprite;
	e_AnimationType      walkingAnim;
	e_AnimationType      attackAnim;
	e_AnimationType      idleAnim;
};

using NPCMap = std::map<uint8_t, NPCDef>;

const static NPCDef get_npc(uint8_t _id) 
{
	using namespace Graphics;

	NPCDef npc;

	switch(_id) 
	{
	case 0:
		npc.name = "Goblin";
		npc.combatLevel = 3;
		npc.m_sprite = SpriteType::NPC_GOBLIN;
		npc.idleAnim    = e_AnimationType::GOBLIN_IDLE;
		npc.walkingAnim = e_AnimationType::GOBLIN_WALK;
		npc.attackAnim  = e_AnimationType::GOBLIN_ATTACK;

	default:
		npc.name = "null";
		npc.combatLevel = 0;
		npc.m_sprite = SpriteType::NONE;
		npc.idleAnim    = e_AnimationType::NO_ANIMATION;
		npc.walkingAnim = e_AnimationType::NO_ANIMATION;
		npc.attackAnim  = e_AnimationType::NO_ANIMATION;
	}

	return npc;
}