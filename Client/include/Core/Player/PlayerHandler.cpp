#include "precomp.h"

#include "PlayerHandler.h"

#include "Core/Global/C_Globals.h"

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

		players[_playerhandle] = _details;
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

PlayerDetails& PlayerHandler::get_details(const uint64_t& _playerhandle)
{
	if(players.find(_playerhandle) == players.end()) 
	{
		DEVIOUS_ERR("Handle " << _playerhandle << " wasn't registered & data doesn't exist.");
	}

	return players[_playerhandle];
}

PlayerDetails& PlayerHandler::get_local_player_details()
{
	DEVIOUS_ASSERT(players.find(localPlayerId) != players.end());
	return players[localPlayerId];
}

void PlayerHandler::register_local_player(uint64_t _localPlayerId)
{
	localPlayerId = _localPlayerId;
	DEVIOUS_EVENT("Indentified our local player to be: " << _localPlayerId);
}

