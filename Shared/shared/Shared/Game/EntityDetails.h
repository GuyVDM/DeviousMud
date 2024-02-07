#pragma once
#include "Shared/Game/Skill.hpp"
#include "Shared/Utilities/vec2.hpp"
#include <string>

using Skills = DEVIOUSMUD::SKILLS::skill;

typedef struct _EntityDetails
{
	uint8_t                   combatLevel;
	Utilities::ivec2          position;
	Skills                    skills[DEVIOUSMUD::SKILLS::SKILL_COUNT];

}   EntityDetails;