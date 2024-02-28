#pragma once
#include "Shared/Game/Skill.hpp"
#include "Shared/Utilities/vec2.hpp"
#include <string>
#include <array>

using Skills = std::array<DEVIOUSMUD::SKILLS::skill, DEVIOUSMUD::SKILLS::SKILL_COUNT>;

typedef struct _EntityDetails
{
	uint8_t           combatLevel;
	Utilities::ivec2  position;
	Skills            skills;

}   EntityDetails;