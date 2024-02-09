#include "precomp.h"

#include "PlayerHandler.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Utilities/Globals.hpp"

#include "Shared/Utilities/UUID.hpp"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Navigation/AStar.h"

#include <cmath>

void Server::PlayerHandler::register_player(const PlayerUUID& _playerId)
{
	if (players.find(_playerId) != players.end())
	{
		DEVIOUS_WARN("A player already exists with this handle. " << _playerId);
		return;
	}

	players[_playerId] = std::make_shared<PlayerDetails>();
	players[_playerId]->handle = _playerId;

	DEVIOUS_EVENT("Player " << _playerId << " has logged in.");
}

void Server::PlayerHandler::logout_player(const PlayerUUID& _playerId)
{
	if (players.find(_playerId) == players.end())
	{
		DEVIOUS_WARN("Couldn't logout player: " << _playerId);
		return;
	}

	DEVIOUS_EVENT("Player: " << _playerId << " has logged out.");
	players.erase(_playerId);

	//TODO: Add RPC to other clients that they should remove the player too locally.
}

void Server::PlayerHandler::add_experience(const PlayerUUID& _playerId, const DEVIOUSMUD::SKILLS::e_skills& _skilltype, const uint32_t& _experience)
{
	if(players.find(_playerId) == players.end()) 
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
		return;
	}

	//Get player reference.
	std::shared_ptr<PlayerDetails> player = players[_playerId];

	//Create skill handle.
	const uint8_t skillhandle = static_cast<uint8_t>(_skilltype);

	//Grab skill
	DEVIOUSMUD::SKILLS::skill* skill = &player->skills[skillhandle];

	//Calculate total exp
	int32_t exp = skill->experience + _experience;

	//Prevent exp from going over the experience cap.
	exp = min(exp, DEVIOUSMUD::GLOBALS::GAMECONFIG::EXPERIENCE_CAP);

	//Apply the experience
	skill->experience = exp;

	//Check if there's a level up.
	if(skill->level < DEVIOUSMUD::GLOBALS::GAMECONFIG::get_level(exp))
	{
		skill->level++;

		if(skill->levelboosted < skill->level) 
		{
			skill->levelboosted++;
		}

		DEVIOUS_LOG("Player " << _playerId << "has leveled up a skill.");
	}
}

bool Server::PlayerHandler::move_player_towards(const PlayerUUID& _playerId, const Utilities::ivec2& _target, const bool& _isRunning)
{
	if (players.find(_playerId) == players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId);
		return false;
	}

	const Utilities::ivec2 playerPos = players[_playerId]->position;

	if (playerPos == _target) 
		return true;

	const std::vector<Utilities::ivec2> path = DEVIOUSMUD::PATH::AStar::find_path(playerPos, _target);

	//Check if there's a path to the destination.
	if (path.size() > 0)
	{
		Utilities::ivec2 nextPos;

		if (_isRunning && path.size() > 1)
		{
			nextPos = path[1];
		}
		else
		{
			nextPos = path[0];
		}

		//Move the player one position.
		set_player_position(_playerId, nextPos);
	}

	return players[_playerId]->position == _target;
}

void Server::PlayerHandler::set_player_position(const PlayerUUID& _playerId, const Utilities::ivec2& _target)
{
	if (players.find(_playerId) == players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
		return;
	}

	players[_playerId]->position = _target;
}

const Utilities::ivec2& Server::PlayerHandler::get_player_position(const PlayerUUID& _playerId)
{
	if (players.find(_playerId) == players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId);
		return { };
	}

	return players[_playerId]->position;
}
