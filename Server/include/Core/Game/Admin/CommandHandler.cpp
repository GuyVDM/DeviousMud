#include "precomp.h"

#include <sstream>

#include "Core/Game/Admin/CommandHandler.h"

#include "Core/Game/Entity/Definition/EntityDef.h"

#include "Core/Globals/S_Globals.h"

#include "Core/Game/Entity/EntityHandler.h"

#include "Core/Network/Connection/ConnectionHandler.h"

bool CommandHandler::try_handle_as_command(std::shared_ptr<Player> _player, const std::string& _string)
{
	const static std::string commandPrefix = "::";

	if(starts_with(_string, commandPrefix))
	{
		const std::string command = _string.substr(commandPrefix.length());

		//*
		// Store all potential command specific parameters into the vector
		//*
		std::vector<std::string> commandArgs;
		{
			std::istringstream sstream(command);

			// Split the string by space and store each word in the vector
			std::string word;
			while (std::getline(sstream, word, ' '))
			{
				commandArgs.push_back(word);
			}
		}

		if(commandArgs[0] == "teleport")
		{
			if(commandArgs.size() > 2)
			{
				int32_t x, y;
				
				if(try_parse_as_int(commandArgs[1], x) &&
				   try_parse_as_int(commandArgs[2], y)) 
				{
					_player->teleport_to(Utilities::ivec2(x, y));
					_player->whisper("<col=#FF0000>[Server]: Teleported player to: " + std::to_string(x) + ", " + std::to_string(y));
					return true;
				}
			}

			_player->whisper("<col=#FF0000>[Server]: Invalid arguments were specified.");
		}

		if(commandArgs[0] == "setrank") 
		{
			std::string failReason = "<col=#FF0000>[Server]: Invalid arguments were specified.";

			if (commandArgs.size() > 2)
			{
				int32_t rank;

				if(try_parse_as_int(commandArgs[1], rank))
				{
					std::string fullName = commandArgs[2];

					if (commandArgs.size() > 3)
					{
						//If there are words to attach back together, do so.
						for (int32_t i = 3; i < commandArgs.size(); i++)
						{
							fullName.append(" ");
							fullName.append(commandArgs[i]);
						}
					}

					auto& cHandler = g_globals.connectionHandler;

					auto& clientHandles = g_globals.connectionHandler->get_client_handles();

					for (uint64_t clientHandle : clientHandles)
					{
						auto optEntt = g_globals.entityHandler->get_entity(clientHandle);

						std::shared_ptr<Player> target = std::static_pointer_cast<Player>(optEntt.value());

						if (target->name == fullName)
						{
							//*--------------------------------
							// Check if the rank given is valid
							//*
							if (rank > -1 && rank < static_cast<int32_t>(Player::e_PlayerRights::RankCount))
							{
								Player::e_PlayerRights newRights = static_cast<Player::e_PlayerRights>(rank);
								target->set_player_rights(newRights);
								return true;
							}
							else failReason = "<col=#FF0000>[Server]: Invalid rank was specified.";
						}
						else failReason = "<col=#FF0000>[Server]: Player with name doesn't exist.";
					}
				}
			}

			_player->whisper(failReason);
			return true;
		}

		if (commandArgs[0] == "changename")
		{
			if (commandArgs.size() > 1)
			{
				std::string fullName = commandArgs[1];

				//If there are words to attach back together, do so.
				if (commandArgs.size() > 2)
				{
					for (int32_t i = 2; i < commandArgs.size(); i++)
					{
						fullName.append(" ");
						fullName.append(commandArgs[i]);
					}
				}

				_player->set_name(fullName);	
			}
			else 
			{
				_player->whisper("<col=#FF0000>[Server]: Invalid arguments were specified.");
			}

			return true;
		}

		if(commandArgs[0] == "kick") 
		{
			if (commandArgs.size() > 1)
			{
				std::string fullName = commandArgs[1];

				//If there are words to attach back together, do so.
				if (commandArgs.size() > 2)
				{
					for (int32_t i = 2; i < commandArgs.size(); i++)
					{
						fullName.append(" ");
						fullName.append(commandArgs[i]);
					}
				}

				auto& cHandler = g_globals.connectionHandler;

				auto& clientHandles = g_globals.connectionHandler->get_client_handles();
				
				for(uint64_t clientHandle : clientHandles) 
				{
					auto optEntt = g_globals.entityHandler->get_entity(clientHandle);

					std::shared_ptr<Player> target = std::static_pointer_cast<Player>(optEntt.value());
						
					if (target->name == fullName)
					{
						_player->whisper("<col=#FF0000>[Server]: Succesfully disconnected: " + target->name + '.');
						cHandler->disconnect_client(static_cast<enet_uint32>(clientHandle));
						return true;
					}
				}
			}

			_player->whisper("<col=#FF0000>[Server]: The player you tried to kick does not exist.");
			return true;
		}

		if(commandArgs[0] == "spawnnpc") 
		{
			if (commandArgs.size() > 1)
			{
				int32_t npcId;
				if (try_parse_as_int(commandArgs[1], npcId))
				{
					if (npcId > 0 && npcId <= UINT8_MAX)
					{
						Utilities::ivec2 playerPos = _player->position;

						g_globals.entityHandler->create_world_npc
						(
							static_cast<uint8_t>(npcId),
							playerPos
						);

						_player->whisper("<col=#FF0000>[Server]: Spawned NPC ID: " + std::to_string(npcId) + " at "
                                          + std::to_string(playerPos.x) + " , " + std::to_string(playerPos.y) + '.');
						return true;
					}
				}
			}

			_player->whisper("<col=#FF0000>[Server]: Invalid arguments were specified.");
			return true;
		}
	}

	return false;
}

bool CommandHandler::starts_with(const std::string& _str, const std::string& _prefix)
{
	return _str.find(_prefix) == 0; // Returns true if the prefix is found at position 0
}

bool CommandHandler::try_parse_as_int(const std::string& _string, int32_t& _value)
{
	try 
	{
		_value = std::stoi(_string);
		return true;
	}
	catch (const std::invalid_argument& e) 
	{
		DEVIOUS_ERR("Invalid argument: " << e.what());
	}
	catch (const std::out_of_range& e) 
	{
		DEVIOUS_ERR("Out of range: " << e.what());
	}

	return false;
}
