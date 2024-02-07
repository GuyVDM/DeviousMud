#include "precomp.h"

#include "PlayerHandler.h"

#include "Core/Global/C_Globals.h"

void PlayerHandler::create_player(const uint64_t& _playerhandle, PlayerDetails& _details)
{
	if (players.find(_playerhandle) == players.end()) 
	{
		std::cout << "[Handlers::PlayerHandler] Created player of handle: " << _playerhandle << std::endl;
		players[_playerhandle] = _details;
		on_player_created.invoke(_playerhandle);
		return;
	}

	std::cout << "[Handlers::PlayerHandler] Player of handle: " << _playerhandle << " already exists." << std::endl;
}

void PlayerHandler::remove_player(const uint64_t& _playerhandle)
{
	if (players.find(_playerhandle) == players.end()) 
	{
		std::cout << "[Handlers::PlayerHandler] Player of handle: " << _playerhandle << " doesn't exist and couldn't get removed." << std::endl;
		return;
	}

	players.erase(_playerhandle);
	on_player_removed.invoke(_playerhandle);

	std::cout << "[Handlers::PlayerHandler] Player of handle: " << _playerhandle << " was removed." << std::endl;
}

PlayerDetails& PlayerHandler::get_details(const uint64_t& _playerhandle)
{
	if(players.find(_playerhandle) == players.end()) 
	{
		std::cout << "[Handlers::PlayerHandler] ERROR: " << _playerhandle << " wasn't registered & data doesn't exist." << std::endl;
	}

	return players[_playerhandle];
}

