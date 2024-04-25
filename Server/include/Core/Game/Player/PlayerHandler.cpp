#include "precomp.h"

#include "PlayerHandler.h"

#include "Shared/Network/Packets/PacketHandler.hpp"

#include "Shared/Utilities/Globals.hpp"

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Navigation/AStar.hpp"

#include "Core/Network/NetworkHandler.h"

#include "Core/Network/Connection/ConnectionHandler.h"

#include "Core/Network/Client/ClientInfo.h"

#include "Core/Events/Query/EventQuery.h"

#include "Core/Globals/S_Globals.h"

#include <cmath>

using Path = std::vector<Utilities::ivec2>;

void Server::EntityHandler::register_player(const PlayerUUID& _playerId)
{
	if (m_players.find(_playerId) != m_players.end())
	{
		DEVIOUS_WARN("A player already exists with this handle. " << _playerId);
		return;
	}

	m_players[_playerId] = std::make_shared<PlayerDetails>();

	DEVIOUS_EVENT("Player " << _playerId << " has logged in.");
}

void Server::EntityHandler::logout_player(const PlayerUUID& _playerId)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_WARN("Couldn't logout player: " << _playerId);
		return;
	}

	DEVIOUS_EVENT("Player: " << _playerId << " has logged out.");
	m_players.erase(_playerId);
}

void Server::EntityHandler::add_experience(const PlayerUUID& _playerId, const DM::SKILLS::e_skills& _skilltype, const uint32_t& _experience)
{
	if(m_players.find(_playerId) == m_players.end()) 
	{
   		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
		return;
	}

	//Get player reference.
	std::shared_ptr<PlayerDetails> player = m_players[_playerId];

	//Create skill handle.
	const uint8_t skillhandle = static_cast<uint8_t>(_skilltype);

	//Grab skill
	DM::SKILLS::Skill* Skill = &player->skills[skillhandle];

	//Calculate total exp
	int32_t exp = Skill->experience + _experience;

	//Prevent exp from going over the experience cap.
	exp = min(exp, DM::GLOBALS::GAMECONFIG::EXPERIENCE_CAP);

	//Apply the experience
	Skill->experience = exp;

	//Check if there's a level up.
	if(Skill->level < DM::GLOBALS::GAMECONFIG::get_level(exp))
	{
		Skill->level++;

		if(Skill->levelboosted < Skill->level) 
		{
			Skill->levelboosted++;
		}

		DEVIOUS_EVENT("Player " << _playerId << "has leveled up a skill.");
	}
}

bool Server::EntityHandler::move_player_towards(const PlayerUUID& _playerId, const Utilities::ivec2& _target, const bool& _isRunning)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId);
		return false;
	}

	const Utilities::ivec2 playerPos = m_players[_playerId]->m_position;
	
	//Check if we already gave a destination that's equal to our current position.
	if (playerPos == _target) 
		return true;

	const std::vector<Utilities::ivec2> m_path = DM::Path::AStar::find_path(playerPos, _target);

	//Check if there's a path to the destination.
	if (m_path.size() > 0)
	{
		Utilities::ivec2 nextPos;

		if (_isRunning && m_path.size() > 1)
		{
			nextPos = m_path[1];
		}
		else
		{
			nextPos = m_path[0];
		}

		//Move the player one position.
		set_player_position(_playerId, nextPos);
	}

	return m_players[_playerId]->m_position == _target;
}

void Server::EntityHandler::set_player_position(const PlayerUUID& _playerId, const Utilities::ivec2& _target)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_WARN("No player data was found with the handle: " << _playerId << ".");
		return;
	}

	m_players[_playerId]->m_position = _target;
}

const Utilities::ivec2 Server::EntityHandler::get_player_position(const PlayerUUID& _playerId)
{
	if (m_players.find(_playerId) == m_players.end())
	{
		DEVIOUS_ERR("No player data was found with the handle given");
		return { Utilities::ivec2(0) };
	}

	return m_players[_playerId]->m_position;
}
