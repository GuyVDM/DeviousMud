#pragma once

#include "Core/Core.h"

#include "Core/Config/Config.h"

#include "Shared/Game/SpriteTypes.hpp"

#include <map>

#include <string>

enum class e_SelectedLayer : U8
{
	LAYER_BACKGROUND = 0,
	LAYER_STRUCTURES,
	LAYER_NPC,
	LAYER_COUNT
};

namespace Layers
{
	inline constexpr static Graphics::SpriteType LayerToSprite(const e_SelectedLayer& _layer)
	{
		using namespace App::Config;

		switch (_layer)
		{
			case e_SelectedLayer::LAYER_BACKGROUND:
				return Graphics::SpriteType::TILE_MAP;

			case e_SelectedLayer::LAYER_STRUCTURES:
				return Graphics::SpriteType::STRUCTURES_MAP;

			case e_SelectedLayer::LAYER_NPC:
				return TileConfiguration.SelectedNPC.sprite;

			default:
				return Graphics::SpriteType::NONE;
		}
	}

	inline constexpr static U32 s_LayerCount = static_cast<U32>(e_SelectedLayer::LAYER_COUNT);

	inline constexpr static std::string GetLayerName(const e_SelectedLayer& _layer)
	{
		switch (_layer)
		{
		case e_SelectedLayer::LAYER_BACKGROUND:
			return "Background";

		case e_SelectedLayer::LAYER_STRUCTURES:
			return "Structures";

		case e_SelectedLayer::LAYER_NPC:
			return "NPCs";

		default:
			return "null";
		}
	}
}