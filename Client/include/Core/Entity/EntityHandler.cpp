#include "precomp.h"

#include "Shared/Navigation/AStar.hpp"

#include "Core/Entity/EntityHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Application/Config/Config.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

using Config = DM::CLIENT::Config;

using namespace DM;
using namespace Path;

using namespace Graphics::Animation;

EntityHandler::EntityHandler()
{
	m_localPlayerId = 0;

	//TODO: Remove this and add the invocation of the on local player assigned instead.
	on_local_player_assigned.add_listener
	(
		std::bind(&EntityHandler::register_local_player, this, std::placeholders::_1)
	);
}

EntityHandler::~EntityHandler()
{
	m_worldEntities.clear();
	m_players.clear();
}

void EntityHandler::create_world_entity(DM::Utils::UUID _uuid, uint8_t _npcId, Utilities::ivec2 _pos)
{
	m_worldEntities[_uuid] = WorldEntity::create_entity(_npcId, _pos, _uuid);
	DEVIOUS_LOG("Created world entity by id:" << _npcId);
}

void EntityHandler::remove_world_entity(DM::Utils::UUID _uuid)
{
	const auto it = m_worldEntities.find(_uuid);

	if(it == m_worldEntities.end()) 
	{
		DEVIOUS_WARN("Couldn't remove NPC: " << _uuid);
		return;
	}

	m_worldEntities.erase(m_worldEntities.find(_uuid));
}


std::optional<RefEntity> EntityHandler::get_entity(DM::Utils::UUID _id) const
{
	if(m_worldEntities.find(_id) == m_worldEntities.end()) 
	{
		return std::nullopt;
	}

	return m_worldEntities.at(_id);
}

RefEntity EntityHandler::get_local_player_data()
{
	DEVIOUS_ASSERT(m_worldEntities.find(m_localPlayerId) != m_worldEntities.end());
	return m_worldEntities[m_localPlayerId];
}

void EntityHandler::register_local_player(uint64_t _localPlayerId)
{
	m_localPlayerId = _localPlayerId;
	
	//We don't want to be able to interact with our local player.
	m_worldEntities[_localPlayerId]->set_interaction_mode(e_InteractionMode::STATE_MASKED);

	DEVIOUS_EVENT("Indentified our local player to be: " << _localPlayerId);
}

void EntityHandler::update()
{
	on_entity_update.invoke();
}