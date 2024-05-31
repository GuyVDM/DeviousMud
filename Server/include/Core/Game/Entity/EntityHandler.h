#pragma once

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include "Core/Game/Entity/Definition/EntityDef.h"

#include <unordered_map>

#include <optional>

#pragma region FORWARD_DECLERATIONS


#pragma endregion

using EntityUUID = DM::Utils::UUID;
using enet_uint32 = unsigned int;

namespace Server 
{
	class EntityHandler
	{
	public:
		/// <summary>
		/// Returns the amount of distance between 2 entities.
		/// Using Chebyshev distance on a grid.
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
		/// <returns></returns>
		static const int32_t get_distance(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b); 


	public:
		/// <summary>
		/// Registers a player.
		/// </summary>
		/// <param name="_playerId"></param>
		void register_player(const uint64_t _clientId);


		/// <summary>
		/// Removes a player their data.
		/// </summary>
		/// <param name="_playerId"></param>
		void logout_player(const uint64_t _playerId);


		/// <summary>
		/// Moves the player to a specific location using Pathfinding.
		/// </summary>
		/// <param name="_playerId"></param>
		/// <param name="_target"></param>
		/// <param name="_isRunning"></param>
		/// <returns></returns>
		bool move_entity_towards(const EntityUUID _entityId,
			                     const Utilities::ivec2 _target, const bool _bIsRunning = false);

		/// <summary>
		/// Adds a new entity to the world.
		/// </summary>
		/// <param name="npcId"></param>
		/// <param name="_pos"></param>
		void create_world_npc(uint8_t npcId, Utilities::ivec2 _pos);


		/// <summary>
		/// Returns the current list of entities that are within the world.
		/// </summary>
		/// <returns></returns>
		const std::vector<std::shared_ptr<NPC>> get_world_npcs();


		/// <summary>
		/// Returns any entity matching the indentifier, will iterate through both players and npc's alike.
		/// </summary>
		/// <param name="_id"></param>
		/// <returns></returns>
		std::optional<std::shared_ptr<Entity>> get_entity(const DM::Utils::UUID& _id);


		/// <summary>
		/// Returns, if valid, the owning client of the player it's handle in 64 bits.
		/// </summary>
		/// <param name="_uuid"></param>
		/// <returns></returns>
		const std::optional<uint64_t> transpose_player_to_client_handle(DM::Utils::UUID _uuid) const;

		void entities_tick();

	public:
		EntityHandler() = default;
		~EntityHandler();

	private:
		/// <summary>
		/// All world entities.
		/// </summary>
		std::unordered_map<EntityUUID, std::shared_ptr<Entity>> m_entities;

		/// <summary>
		/// To indentify which handles are npc's
		/// </summary>
		std::vector<EntityUUID> m_npcHandles;

		/// <summary>
		/// Traces the playerId back to the ClientId.
		/// </summary>
		std::unordered_map<uint64_t, DM::Utils::UUID> m_playerToClientHandles;
	};
}
