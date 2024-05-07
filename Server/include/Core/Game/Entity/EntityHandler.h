#pragma once

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include "Core/Game/Entity/Definition/EntityDef.h"

#include <unordered_map>

#pragma region FORWARD_DECLERATIONS

typedef unsigned int enet_uint32;

#pragma endregion

using EntityUUID = DM::Utils::UUID;

namespace Server 
{
	class EntityHandler
	{
	public:
		/// <summary>
		/// Registers a player.
		/// </summary>
		/// <param name="_playerId"></param>
		void register_player(const EntityUUID _playerId);



		/// <summary>
		/// Removes a player their data.
		/// </summary>
		/// <param name="_playerId"></param>
		void logout_player(const EntityUUID _playerId);



		/// <summary>
		/// Moves the player to a specific location using Pathfinding.
		/// </summary>
		/// <param name="_playerId"></param>
		/// <param name="_target"></param>
		/// <param name="_isRunning"></param>
		/// <returns></returns>
		bool move_player_towards(const EntityUUID _playerId,
			                     const Utilities::ivec2 _target, const bool& _isRunning);


		/// <summary>
		/// Sets the player its position directly.
		/// </summary>
		/// <param name="_playerId"></param>
		/// <param name="_target"></param>
		void set_player_position(const EntityUUID _playerId,
								 const Utilities::ivec2 _target);


		/// <summary>
		/// Returns the position of the player.
		/// </summary>
		/// <param name="_playerId"></param>
		/// <returns></returns>
		const Utilities::ivec2 get_player_position(const EntityUUID _playerId);


		/// <summary>
		/// Adds a new entity to the world.
		/// </summary>
		/// <param name="npcId"></param>
		/// <param name="_pos"></param>
		void create_entity(uint8_t npcId, Utilities::ivec2 _pos);


		/// <summary>
		/// Moves the NPC to a certain location.
		/// </summary>
		/// <param name="_npc"></param>
		/// <param name="_pos"></param>
		bool move_entity_towards(NPC& _npc, Utilities::ivec2 _target);


		/// <summary>
		/// Returns the current list of entities that are within the world.
		/// </summary>
		/// <returns></returns>
		const std::unordered_map<EntityUUID, std::shared_ptr<NPC>>& get_world_entities();

		void entities_tick();

	public:
		EntityHandler() = default;
		~EntityHandler();

	private:
		std::unordered_map<EntityUUID, std::shared_ptr<Player>> m_players;
		std::unordered_map<EntityUUID, std::shared_ptr<NPC>>    m_entities;
	};
}
