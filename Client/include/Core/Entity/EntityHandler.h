#pragma once
#include "Shared/Game/PlayerDetails.h"

#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/UUID.hpp"

#include "Core/Rendering/Sprite/Sprite.h"

#include "Core/Entity/WorldEntity/WorldEntity.h"

#include "Core/Entity/Simulation/SimPosition.h"

#include <unordered_map>

#pragma region FORWARD_DECLERATIONS
namespace Graphics 
{
	class Renderer;

	namespace UI 
	{
		class EntityLayer;
	}
}

#pragma endregion

/// <summary>
/// Holds data to simulating the player movement, alongside entity stats.
/// </summary>
struct Player 
{ 
	PlayerDetails    m_details;
};

/// <summary>
/// Tracks everything that's related to the players that are registered.
/// </summary>
class EntityHandler 
{
public:
	EventListener<uint64_t> on_local_player_assigned;

	EventListener<uint64_t> on_entity_created;

	EventListener<uint64_t> on_entity_removed;

	RefEntity get_local_player_data();

	void create_world_entity(DM::Utils::UUID _uuid, uint8_t _npcId, Utilities::ivec2 _pos);

	void remove_world_entity(DM::Utils::UUID _uuid);

	void update();

	std::optional<RefEntity> get_entity(DM::Utils::UUID _id) const;

	const std::unordered_map<DM::Utils::UUID, RefEntity>& get_entities();

public:
	EntityHandler();
	~EntityHandler();

private:
	EntityHandler(const EntityHandler&) = delete;

private:
	void register_local_player(uint64_t _localPlayerId);

private:
	std::unordered_map<DM::Utils::UUID, RefEntity>  m_worldEntities;

	uint64_t                                        m_localPlayerId;

	friend Graphics::UI::EntityLayer;
};