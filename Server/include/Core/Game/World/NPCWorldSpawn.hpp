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

	/// <summary>
	/// How long it takes for the static npc to respawn.
	/// </summary>
	int32_t ticksTillRespawn = 20;
};

inline const static std::vector<NPCSpawn> get_world_spawns()
{
	static const std::vector<NPCSpawn> spawns =
	{
		{ 1, Utilities::ivec2(5, 5), 20 } //Goblin;
	};

	return spawns;
}