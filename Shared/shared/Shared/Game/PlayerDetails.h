#pragma once
#include "Shared/Utilities/vec2.hpp"

#include "Shared/Game/Skill.hpp"

struct PlayerDetails
{
	std::string name = "";
	uint8_t combatLevel = 0;
	Utilities::ivec2 m_position  = Utilities::ivec2(0);
	DM::SKILLS::SkillMap skills;
};