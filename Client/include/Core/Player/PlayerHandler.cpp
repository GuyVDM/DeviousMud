#include "precomp.h"

#include "Shared/Navigation/AStar.hpp"

#include "Core/Player/PlayerHandler.h"

#include "Core/Application/Config/Config.h"

#include "Core/Global/C_Globals.h"

using Config = DEVIOUSMUD::CLIENT::Config;

using namespace DEVIOUSMUD;
using namespace PATH;

PlayerHandler::PlayerHandler()
{
	localPlayerId = 0;

	//TODO: Remove this and add the invocation of the on local player assigned instead.
	on_local_player_assigned.add_listener
	(
		std::bind(&PlayerHandler::register_local_player, this, std::placeholders::_1)
	);
}

PlayerHandler::~PlayerHandler()
{
	players.clear();
}

void PlayerHandler::create_player(const uint64_t& _playerhandle, PlayerDetails& _details)
{
	if (players.find(_playerhandle) == players.end()) 
	{
		DEVIOUS_LOG("Created player of handle: " << _playerhandle);

		players[_playerhandle] =
		{
			PlayerDetails(_details),
			SimPosition(Utilities::to_vec2(_details.position))
		};

		on_player_created.invoke(_playerhandle);
		return;
	}

	DEVIOUS_WARN("Player of handle : " << _playerhandle << " already exists.");
}

void PlayerHandler::remove_player(const uint64_t& _playerhandle)
{
	if (players.find(_playerhandle) == players.end()) 
	{
		DEVIOUS_WARN("Player of handle : " << _playerhandle << " doesn't exist and couldn't get removed.");
		return;
	}

	players.erase(_playerhandle);
	on_player_removed.invoke(_playerhandle);

	DEVIOUS_LOG("Player of handle : " << _playerhandle << " was removed.");
}

PlayerData& PlayerHandler::get_data(const uint64_t& _playerhandle)
{
	if(players.find(_playerhandle) == players.end()) 
	{
		DEVIOUS_ERR("Handle " << _playerhandle << " wasn't registered & data doesn't exist.");
	}

	return players[_playerhandle];
}

PlayerData& PlayerHandler::get_local_player_data()
{
	DEVIOUS_ASSERT(players.find(localPlayerId) != players.end());
	return players[localPlayerId];
}

void PlayerHandler::register_local_player(uint64_t _localPlayerId)
{
	localPlayerId = _localPlayerId;
	DEVIOUS_EVENT("Indentified our local player to be: " << _localPlayerId);
}

void _PlayerData::set_position_from_server(const Utilities::ivec2 _position)
{
	details.position = _position;
	simPos.set_target(Utilities::to_vec2(_position));
}

const Utilities::vec2& _PlayerData::get_position() const
{
	return simPos.get_position();
}

void SimPosition::set_target(const Utilities::vec2& _target)
{
	bIsDirty = true;
	timeline = 0.0f;

	// Update positions
	{
		startPos = currentPos;
		endPos = _target;
		
		path = AStar::find_path
		(
			Utilities::to_ivec2(startPos), 
			Utilities::to_ivec2(endPos)
		);
	}
}

void SimPosition::update()
{
	const float MAX_TIMELINE = 1.0f;
	const float TILE_DURATION = 1.0f / path.size();

	//TODO: replace the 0.6 with the gametick amount in seconds.
	timeline = CLAMP(timeline + (Config::get_deltaTime() * 1.6f), 0.000001f, 1.0f);

	//Calculate which tile we're walking towards.
	uint32_t targetTile = (uint32_t)ceilf(timeline / (1.0f / path.size())) - 1;

	// Calculate the current position.
	{
		//Check whether we already passed by one of the tiles, if not, use the start position.
		const Utilities::vec2 TILE_START = targetTile == 0 ? startPos : Utilities::to_vec2(path[targetTile - 1]);

		const Utilities::vec2 TILE_END = Utilities::to_vec2(path[targetTile]);

		const float LOCAL_TIMELINE = (timeline - (targetTile * TILE_DURATION)) / TILE_DURATION;

		currentPos = Utilities::vec2::lerp(TILE_START, TILE_END, LOCAL_TIMELINE);
	}

	if(timeline == MAX_TIMELINE) 
	{
		DEVIOUS_LOG("Synchronization is complete.")
		bIsDirty = false;
	}
}

const bool& SimPosition::is_dirty() const
{
	return bIsDirty;
}

const Utilities::vec2& SimPosition::get_position() const
{
	return currentPos;
}

SimPosition::SimPosition(const Utilities::_vec2& _startPos)
{
	bIsDirty = false;
	timeline = 0.0f;
	startPos = _startPos;
	currentPos = _startPos;
	endPos = _startPos;
}

