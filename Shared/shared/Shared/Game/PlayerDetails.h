#pragma once
#include "Shared/Utilities/vec2.hpp"

#include "Shared/Game/Skill.hpp"

using SkillMap = std::array<DM::SKILLS::Skill, DM::SKILLS::SKILL_COUNT>;

struct PlayerDetails
{
	std::string name = "";
	uint8_t combatLevel = 0;
	Utilities::ivec2 m_position  = Utilities::ivec2(0);
	SkillMap skills;
};