#include "precomp.h"

#include "Shared/Navigation/AStar.hpp"

#include "Core/Player/PlayerHandler.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Application/Config/Config.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

using Config = DEVIOUSMUD::CLIENT::Config;

using namespace DEVIOUSMUD;
using namespace PATH;

using namespace Graphics::Animation;

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
			SimPosition(Utilities::to_vec2(_details.position)),
			g_globals.renderer.lock()->get_sprite(Graphics::SpriteType::PLAYER)
		};
		
		PlayerData& data = players[_playerhandle];
		auto stop_walk_anim = [&data]()
		{
			Animator::stop_current_animation(data.sprite);
		};

		data.simPos.on_reached_dest.add_listener
		(
			stop_walk_anim
		);

		Animator::set_default_animation(players[_playerhandle].sprite, e_AnimationType::PLAYER_IDLE, 8.5f);

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
	if (details.position != _position)
	{
		details.position = _position;
		simPos.set_target(Utilities::to_vec2(_position));
		sprite.bIsFlipped = simPos.get_position().x > details.position.x;

		Animator::play_animation(sprite, e_AnimationType::PLAYER_WALKING, true, 10.0f);
	}
}

void SimPosition::set_target(const Utilities::vec2& _target)
{
	if (Utilities::to_ivec2(_target) == Utilities::to_ivec2(endPos))
		return;

	bIsDirty = true;
	elapsedTime = 0.0f;

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
	const float DURATION_TO_TILE = 1.0f / path.size();

	elapsedTime += DEVIOUSMUD::CLIENT::Config::get_deltaTime();

	float timeline = CLAMP(elapsedTime, 0.0f, 1.0f);

	//Calculate which tile we're walking towards.
	uint32_t targetTile = (uint32_t)ceilf(timeline / (1.0f / path.size())) - 1;

	// Calculate the current position.
	{
		//Check whether we already passed by one of the tiles, if not, use the start position.
		const Utilities::vec2 TILE_START = targetTile == 0 ? startPos : Utilities::to_vec2(path[targetTile - 1]);

		const Utilities::vec2 TILE_END = Utilities::to_vec2(path[targetTile]);

		const float LOCAL_TIMELINE = (timeline - (targetTile * DURATION_TO_TILE)) / DURATION_TO_TILE;

		//const Utilities::vec2 offset = Utilities::vec2(0.25f);
		currentPos = Utilities::vec2::lerp(TILE_START, TILE_END, LOCAL_TIMELINE);
	}

	if(timeline == MAX_TIMELINE) 
	{
		bIsDirty = false;
		on_reached_dest.invoke();
	}
}

const bool& SimPosition::is_dirty() const
{
	return bIsDirty;
}

const Utilities::vec2 SimPosition::get_position() const
{
	const Utilities::vec2 offset = Utilities::vec2(0.25f);
	return currentPos + offset;
}

SimPosition::SimPosition()
{
	bIsDirty = false;
	elapsedTime = 0.0f;
	startPos = Utilities::vec2(0.0f);
	currentPos = startPos;
	endPos = startPos;
}

SimPosition::SimPosition(const Utilities::_vec2& _startPos)
{
	bIsDirty = false;
	elapsedTime = 0.0f;
	startPos = _startPos;
	currentPos = _startPos;
	endPos = _startPos;
}

