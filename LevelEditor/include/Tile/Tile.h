#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"

enum class e_EntityType
{
	ENTITY_SCENIC = 0,
	ENTITY_NPC
};

struct Tile
{
	Utilities::ivec2 LocalChunkCoords;
	Utilities::ivec2 ChunkCoords;
	e_EntityType     EntityType;
	bool             bIsWalkable;

	Tile()
	{
		ChunkCoords      = Utilities::ivec2(0);
		EntityType  = e_EntityType::ENTITY_SCENIC;
		bIsWalkable = true;
	};

	virtual void Render() = 0;
};

struct ScenicTile : public Tile
{
	U16  SpriteHandle;

	ScenicTile();

	virtual void Render() override;
};

struct NPCTile : public Tile
{
	U8    NPCId;
	float RespawnTimer;

	NPCTile()
	{
		NPCId = 0;
		RespawnTimer = 1.0f;
	};

	virtual void Render() override;
};





