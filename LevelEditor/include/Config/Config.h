#pragma once

#include "Core/Core.hpp"

struct Tile;
struct ScenicTile;
struct ObjectTile;
struct NPC;

enum class e_EntityType;

namespace Graphics 
{
	enum class SpriteType : uint16_t;
}

namespace App 
{
	namespace Config 
	{
		const U32 GRIDSIZE = 32;

		struct TileConfig 
		{
			e_EntityType CurrentTileType;

			/* Tile Variables */
			Graphics::SpriteType SpriteType;
			bool                 bIsWalkable;
			U32                  RenderOrder;

			/* NPC Specific Variables */
			float        NPCRespawnTime;
			U32          NPCId;
		};

		extern TileConfig TileConfiguration;

		class EditorConfig
		{
		public:
			static float GetDT();
			static void  UpdateDT();

		private:
			static float s_DeltaTime;
			static std::chrono::time_point<std::chrono::high_resolution_clock> s_LastTime;
		};
	}
}
