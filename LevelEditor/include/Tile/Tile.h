#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"

constexpr U32 TILE_SIZE  = 32;
constexpr U32 CHUNK_SIZE = 32;

enum class e_EntityType
{
	ENTITY_SCENIC = 0,
	ENTITY_OBJECT,
	ENTITY_NPC
};

struct Tile
{
	Utilities::ivec2 Coords;
	e_EntityType     EntityType;

	Tile()
	{
		Coords = Utilities::ivec2(0);
		EntityType = e_EntityType::ENTITY_SCENIC;
	};

	virtual void Render() = 0;
};

struct ScenicTile : public Tile
{
	U16  SpriteHandle;
	bool bWalkable;

	ScenicTile()
	{
		SpriteHandle = 0;
		bWalkable = true;
	}

	virtual void Render() override;
};

struct ObjectTile : public Tile
{
	U16 ObjectId;

	ObjectTile()
	{
		ObjectId = 0;
	}

	virtual void Render() override;
};

struct NPCTile : public Tile
{
	U16   NPCId;
	float RespawnTimer;

	NPCTile()
	{
		NPCId = 0;
		RespawnTimer = 1.0f;
	};

	virtual void Render() override;
};





