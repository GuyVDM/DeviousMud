#pragma once
#include "Core/Core.hpp"

#include "Core/Renderer/SubSprite/SubSprite.hpp"

#include "Shared/Utilities/vec2.hpp"

enum class e_EntityType
{
	ENTITY_DEFAULT = 0,
	ENTITY_NPC
};

struct Tile
{
	SubSprite            Sprite;
	Utilities::ivec2     LocalChunkCoords;
	Utilities::ivec2     ChunkCoords;
	bool                 bIsWalkable;

	Tile(Tile* _other) 
	{
		Sprite			 = _other->Sprite;
		LocalChunkCoords = _other->LocalChunkCoords;
		ChunkCoords		 = _other->ChunkCoords;
		bIsWalkable		 = _other->bIsWalkable;
	}

	Tile()
	{
		Sprite	         = SubSprite(Graphics::SpriteType::NONE, 0);
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





