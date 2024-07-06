#include "precomp.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "Config/Config.h"

#include "Tile/Tile.h"

#include "Shared/Game/SpriteTypes.hpp"

App::Config::TileConfig App::Config::TileConfiguration = TileConfig
(
	/* CurrentTileType */ e_EntityType::ENTITY_SCENIC,
	/* SpriteType      */ static_cast<Graphics::SpriteType>(1),
	/* isWalkable      */ true,
	/* RenderOrder     */ 0,
	/* NPCRespawnTime  */ 1.0f,
	/* NPCId           */ 1
);

App::Config::SettingsConfig App::Config::SettingsConfiguration = SettingsConfig
(
	/* bShowWalkableTiles */ false
);


#pragma region EDITOR_CONFIG
float App::Config::EditorConfig::s_DeltaTime;

std::chrono::time_point<std::chrono::high_resolution_clock> App::Config::EditorConfig::s_LastTime;

float App::Config::EditorConfig::GetDT()
{
	return s_DeltaTime;
}

void App::Config::EditorConfig::UpdateDT()
{
	const float MAX_DELTATIME = 0.1f;

	const auto current_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - s_LastTime);
	s_LastTime = current_time;

	const float s = static_cast<float>(duration.count() / 1000000.0f);

	s_DeltaTime = std::min(s, MAX_DELTATIME);
}
#pragma endregion
