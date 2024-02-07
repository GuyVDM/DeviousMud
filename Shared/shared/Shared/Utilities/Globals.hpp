#pragma once
#include <random>

namespace DEVIOUSMUD
{
	namespace UTILITIES 
	{
		uint64_t generate_handle_u64()
		{
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<uint64_t> dis;
			return dis(gen);
		};
	}

#include <cmath>
	namespace GLOBALS
	{
		namespace GAMECONFIG 
		{
			constexpr uint8_t MIN_LEVEL = 1, MAX_LEVEL = 99;

			constexpr uint32_t EXPERIENCE_CAP = 200000000;

			/// <summary>
			/// Source: https://oldschool.runescape.wiki/w/Experience#:~:text=The%20amount%20of%20experience%20needed,a%20total%20of%2013%2C034%2C431%20experience.
			/// </summary>
			/// <param name="_experience"></param>
			/// <returns></returns>
			uint8_t get_level(uint32_t _experience) 
			{
					uint8_t level = 1;
					uint32_t totalExperience = 0;

					while (level < MAX_LEVEL)
					{
						uint32_t expNeeded = static_cast<uint32_t>((1.0 / 4) * std::floor(level + 300 * pow(2, level / 7.0)));
						totalExperience += expNeeded;

						if (totalExperience > _experience)
						{
							break;
						}

						++level;
					}

					return level;
			}
		}
	};
};