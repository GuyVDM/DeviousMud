#pragma once

#include "Core/Core.hpp"

struct Tile;
struct ScenicTile;
struct ObjectTile;
struct NPC;

namespace App 
{
	namespace Config 
	{
		class Configuration
		{
		public:
			static float GetDT();
			static void  UpdateDT();

		private:
			static float s_DeltaTime;
			static std::chrono::time_point<std::chrono::high_resolution_clock> s_LastTime;
		};

		const U32 GRIDSIZE = 32;
	}
}
