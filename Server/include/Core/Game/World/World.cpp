#include "precomp.h"

#include "Core/Game/World/World.h"

#include "Core/Game/World/NPCWorldSpawn.hpp"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Globals/S_Globals.h"

void Server::World::init()
{
	//Load in all Static World Entities.
	for (const NPCSpawn& spawn : get_world_spawns())
	{
		//TODO: Handle respawning
		g_globals.entityHandler->create_entity(spawn.npcId, spawn.spawnCoords);
		DEVIOUS_EVENT("Spawned NPC ID: " << spawn.npcId << " into the world at: " << spawn.spawnCoords.x << ", " << spawn.spawnCoords.y << ".")
	}
}
