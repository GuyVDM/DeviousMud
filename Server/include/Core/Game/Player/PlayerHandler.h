#pragma once

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Game/PlayerDetails.h"

#include "Shared/Utilities/vec2.hpp"

#include <unordered_map>

#pragma region FORWARD_DECLERATIONS

typedef unsigned int enet_uint32;

#pragma endregion

using PlayerUUID = DEVIOUSMUD::RANDOM::UUID;

namespace Server 
{
	class PlayerHandler
	{
	public:
		void register_player(const PlayerUUID& _playerId);

		void logout_player(const PlayerUUID& _playerId);

		void add_experience(const PlayerUUID& _playerId,
							const DEVIOUSMUD::SKILLS::e_skills& _skilltype,
							const uint32_t& _experience);

		bool move_player_towards(const PlayerUUID& _playerId,
			                     const Utilities::ivec2& _target, const bool& _isRunning);

		void set_player_position(const PlayerUUID& _playerId,
								 const Utilities::ivec2& _target);

		const Utilities::ivec2& get_player_position(const PlayerUUID& _playerId);

	private:
		std::unordered_map<PlayerUUID, std::shared_ptr<PlayerDetails>> players;
	};
}
