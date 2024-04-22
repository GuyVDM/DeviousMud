#pragma once
#include "Shared/Game/Skill.hpp"
#include "Shared/Utilities/vec2.hpp"
#include <string>
#include <array>

using Skills = std::array<DM::SKILLS::Skill, DM::SKILLS::SKILL_COUNT>;

struct EntityDetails
{
	uint8_t           combatLevel;
	Utilities::ivec2  m_position;
	Skills            skills;

};