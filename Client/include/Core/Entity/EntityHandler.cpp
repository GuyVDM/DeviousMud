#include "precomp.h"

#include "Shared/Navigation/AStar.hpp"

#include "Core/Entity/EntityHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Application/Config/Config.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

using Config = DM::CLIENT::Config;

using namespace DM;
using namespace PATH;

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
	m_players.clear();
}

void EntityHandler::create_player(const uint64_t _playerhandle, PlayerDetails& _details)
{
	if (m_players.find(_playerhandle) == m_players.end()) 
	{
		DEVIOUS_LOG("Created player of handle: " << _playerhandle);

		m_players[_playerhandle] =
		{
			PlayerDetails(_details),
			SimPosition(Utilities::to_vec2(_details.m_position)),
			g_globals.m_renderer.lock()->get_sprite(Graphics::SpriteType::PLAYER)
		};
		
		PlayerData& data = m_players[_playerhandle];
		auto stop_walk_anim = [&data]()
		{
			Animator::stop_current_animation(data.m_sprite);
		};

		data.m_simPos.on_reached_dest.add_listener
		(
			stop_walk_anim
		);

		Animator::set_default_animation(m_players[_playerhandle].m_sprite, e_AnimationType::PLAYER_IDLE, 8.5f);

		on_player_created.invoke(_playerhandle);
		return;
	}

	DEVIOUS_WARN("Player of handle : " << _playerhandle << " already exists.");
}

void EntityHandler::remove_player(const uint64_t _playerhandle)
{
	if (m_players.find(_playerhandle) == m_players.end()) 
	{
		DEVIOUS_WARN("Player of handle : " << _playerhandle << " doesn't exist and couldn't get removed.");
		return;
	}

	m_players.erase(_playerhandle);
	on_player_removed.invoke(_playerhandle);

	DEVIOUS_LOG("Player of handle : " << _playerhandle << " was removed.");
}

PlayerData& EntityHandler::get_data(const uint64_t _playerhandle)
{
	if(m_players.find(_playerhandle) == m_players.end()) 
	{
		DEVIOUS_ERR("Handle " << _playerhandle << " wasn't registered & data doesn't exist.");
	}

	return m_players[_playerhandle];
}

PlayerData& EntityHandler::get_local_player_data()
{
	DEVIOUS_ASSERT(m_players.find(m_localPlayerId) != m_players.end());
	return m_players[m_localPlayerId];
}

void EntityHandler::register_local_player(uint64_t _localPlayerId)
{
	m_localPlayerId = _localPlayerId;
	DEVIOUS_EVENT("Indentified our local player to be: " << _localPlayerId);
}

void _PlayerData::set_position_from_server(const Utilities::ivec2 _position)
{
	if (m_details.m_position != _position)
	{
		m_details.m_position = _position;
		m_simPos.set_target(Utilities::to_vec2(_position));
		m_sprite.bIsFlipped = m_simPos.get_position().x > m_details.m_position.x;

		Animator::play_animation(m_sprite, e_AnimationType::PLAYER_WALKING, true, 10.0f);
	}
}

void SimPosition::set_target(const Utilities::vec2& _target)
{
	if (Utilities::to_ivec2(_target) == Utilities::to_ivec2(m_endPos))
		return;

	m_bIsDirty = true;
	m_elapsedTime = 0.0f;

	// Update positions
	{
		m_startPos = m_currentPos;
		m_endPos = _target;
		
		m_path = AStar::find_path
		(
			Utilities::to_ivec2(m_startPos), 
			Utilities::to_ivec2(m_endPos)
		);
	}
}

void SimPosition::update()
{
	const float MAX_TIMELINE = 1.0f;
	const float DURATION_TO_TILE = 1.0f / m_path.size();

	m_elapsedTime += DM::CLIENT::Config::get_deltaTime();

	float timeline = CLAMP(m_elapsedTime, 0.0f, 1.0f);

	//Calculate which tile we're walking towards.
	uint32_t targetTileIndex = (uint32_t)ceilf(timeline / (1.0f / m_path.size())) - 1;

	// Calculate the current position.
	{
		//Check whether we already passed by one of the tiles, if not, use the start position.
		const Utilities::vec2 START_TILE = targetTileIndex == 0 ? m_startPos : Utilities::to_vec2(m_path[targetTileIndex - 1]);

		const Utilities::vec2 END_TILE = Utilities::to_vec2(m_path[targetTileIndex]);

		const float LOCAL_TIMELINE = (timeline - (targetTileIndex * DURATION_TO_TILE)) / DURATION_TO_TILE;

		m_currentPos = Utilities::vec2::lerp(START_TILE, END_TILE, LOCAL_TIMELINE);
	}

	if(timeline == MAX_TIMELINE) 
	{
		m_bIsDirty = false;
		on_reached_dest.invoke();
	}
}

const bool& SimPosition::is_dirty() const
{
	return m_bIsDirty;
}

//TODO: Remove the offset and base it all on the way it's rendered.
const Utilities::vec2 SimPosition::get_position() const
{
	//Offset to get the player icon centric to the tile.
	const Utilities::vec2 offset = Utilities::vec2(0.25f);
	return m_currentPos + offset;
}

SimPosition::SimPosition()
{
	m_bIsDirty = false;
	m_elapsedTime = 0.0f;
	m_startPos = Utilities::vec2(0.0f);
	m_currentPos = m_startPos;
	m_endPos = m_startPos;
}

SimPosition::SimPosition(const Utilities::_vec2& _startPos)
{
	m_bIsDirty = false;
	m_elapsedTime = 0.0f;
	m_startPos = _startPos;
	m_currentPos = _startPos;
	m_endPos = _startPos;
}

