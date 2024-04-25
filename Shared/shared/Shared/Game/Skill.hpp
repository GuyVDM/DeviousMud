#pragma once

#include <algorithm>

#include <map>

namespace DM 
{
	namespace SKILLS
	{
		typedef unsigned char uint8_t;  //8 bits
		typedef signed int    int32_t;  //32 bits

		//---All available skills---//
		enum class e_skills : uint8_t
		{
			HITPOINTS = 0x00,
			ATTACK = 0x01,
			DEFENCE = 0x02,
			STRENGTH = 0x03,
			RANGED = 0x04,
			PRAYER = 0x05,
			MAGIC = 0x06,
			COOKING = 0x07,
			WOODCUTTING = 0x08,
			FLETCHING = 0x09,
			FISHING = 0x0A,
			FIREMAKING = 0x0B,
			CRAFTING = 0x0C,
			SMITHING = 0x0D,
			MINING = 0x0E,
			HERBLORE = 0x0F,
			AGILITY = 0x10,
			THIEVING = 0x11,
			SKILL_COUNT
		};  constexpr uint8_t SKILL_COUNT = static_cast<uint8_t>(e_skills::SKILL_COUNT) - 1;

		struct Skill
		{
			uint32_t level = 1;         //Gets calculated when values gets deserialized based on the amount of experience.
			uint32_t levelboosted = 1; //The actual working level that gets used for calculations.
			uint32_t experience = 64;  //The amount of experience accumulated in a skill.
		};

		/// <summary>
		/// Wrapper class for the std::Map to allow the enum to be used as a indexing operator.
		/// </summary>
		class SkillMap final
		{
		public:
			/// <summary>
			/// Source:
			/// https://oldschool.runescape.wiki/w/Combat_level#:~:text=Calculating%20combat%20level&text=Add%20your%20Strength%20and%20Attack,have%20your%20melee%20combat%20level.&text=Multiply%20this%20by%200.325%20and,have%20your%20magic%20combat%20level.
			/// </summary>
			/// <returns></returns>
			//const uint32_t get_combat_level() const
			//{
			//	const uint32_t hitpointsLvl = m_skills.at(e_skills::HITPOINTS).level;
			//	const uint32_t attackLvl    = m_skills.at(e_skills::ATTACK).level;
			//	const uint32_t strLvl       = m_skills.at(e_skills::STRENGTH).level;
			//	const uint32_t defLvl       = m_skills.at(e_skills::DEFENCE).level;
			//	const uint32_t prayLvl      = m_skills.at(e_skills::PRAYER).level;
			//	const uint32_t mageLvl      = m_skills.at(e_skills::MAGIC).level;
			//	const uint32_t rangeLvl     = m_skills.at(e_skills::RANGED).level;

			//	//Calculating combat level.

			//	uint32_t base = (uint32_t)(((float)hitpointsLvl + (float)defLvl) + floor((float)prayLvl / 2.0f));

			//	uint32_t meleeCombatLevel = base + (uint32_t)((float)(strLvl + attackLvl) * 0.325f);

			//	uint32_t mageCombatLevel = base + ((uint32_t)(floor(mageLvl / 2.0f) + (float)mageLvl) * 0.325f);

			//	uint32_t rangeCombatLevel = base + ((uint32_t)(floor(rangeLvl / 2.0f) + (float)rangeLvl) * 0.325f);

			//	return std::max({ meleeCombatLevel, mageCombatLevel, rangeCombatLevel });
			//}

			inline Skill& operator[](DM::SKILLS::e_skills _skill)
			{
				return m_skills[_skill];
			}

			inline std::map<e_skills, DM::SKILLS::Skill>& get_map()
			{
				return m_skills;
			}

		private:
			std::map<e_skills, Skill> m_skills;
		};
	}
}