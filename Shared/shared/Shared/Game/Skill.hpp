#pragma once

namespace DM 
{
	namespace SKILLS
	{
		typedef unsigned char uint8_t;  //8 bits
		typedef signed int    int32_t;  //32 bits

		//---All available skills---//
		enum class e_skills : uint8_t
		{
			HITPOINTS   = 0x00,
			ATTACK      = 0x01,
			DEFENCE     = 0x02,
			STRENGTH    = 0x03, 
			RANGED      = 0x04,
			PRAYER      = 0x05,
			MAGIC       = 0x06,
			COOKING     = 0x07,
			WOODCUTTING = 0x08,
			FLETCHING   = 0x09,
			FISHING     = 0x0A,
			FIREMAKING  = 0x0B,
			CRAFTING    = 0x0C,
			SMITHING    = 0x0D,
			MINING      = 0x0E,
			HERBLORE    = 0x0F,
			AGILITY     = 0x10,
			THIEVING    = 0x11,
			SKILL_COUNT 
		};  constexpr uint8_t SKILL_COUNT = static_cast<uint8_t>(e_skills::SKILL_COUNT) - 1;

		struct Skill
		{
			uint8_t level;         //Gets calculated when values gets deserialized based on the amount of experience.
			uint8_t levelboosted; //The actual working level that gets used for calculations.
			int32_t experience;  //The amount of experience accumulated in a skill.
		};
	}
}