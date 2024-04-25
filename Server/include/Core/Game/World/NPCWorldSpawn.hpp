#include "Shared/Utilities/vec2.hpp"

#include <cstdint>

#include <vector>

struct NPCSpawn
{
	/// <summary>
	/// What NPC to spawn.
	/// </summary>
	uint32_t npcId = 0;

	/// <summary>
	/// Where to spawn the NPC.
	/// </summary>
	Utilities::ivec2 spawnCoords = Utilities::ivec2(0, 0);
};

inline const static std::vector<NPCSpawn> get_world_spawns()
{
	static const std::vector<NPCSpawn> spawns =
	{
		{ 1, Utilities::ivec2(5, 5) } //Goblin;
	};

	return spawns;
}