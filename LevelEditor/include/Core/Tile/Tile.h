#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"
#include "Shared/Game/SpriteTypes.hpp"

enum class e_EntityType
{
	ENTITY_DEFAULT = 0,
	ENTITY_NPC
};

struct Tile
{
	Graphics::SpriteType sprite;
	Utilities::ivec2     LocalChunkCoords;
	Utilities::ivec2     ChunkCoords;
	bool                 bIsWalkable;

	Tile(Tile* _other) 
	{
		sprite           = _other->sprite;
		LocalChunkCoords = _other->LocalChunkCoords;
		ChunkCoords		 = _other->ChunkCoords;
		bIsWalkable		 = _other->bIsWalkable;
	}

	Tile()
	{
		sprite           = Graphics::SpriteType::NONE;
		LocalChunkCoords = Utilities::ivec2(0);
		ChunkCoords      = Utilities::ivec2(0);
		bIsWalkable      = true;
	};

	virtual void Render();
};

struct NPCTile : public Tile
{
	U32   NPCId;
	float RespawnTimer;

	NPCTile()
	{
		NPCId = 0;
		RespawnTimer = 1.0f;
	};

	virtual void Render() override;
};





