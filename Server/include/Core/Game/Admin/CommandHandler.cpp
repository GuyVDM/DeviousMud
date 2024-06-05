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
					_player->whisper("[Server]: Teleported player to: " + std::to_string(x) + ", " + std::to_string(y));
					return true;
				}
			}

			_player->whisper("[Server]: Invalid arguments were specified.");
		}

		if (commandArgs[0] == "changename")
		{
			if (commandArgs.size() > 1)
			{
				std::string name = commandArgs[1];
				{
					_player->set_name(name);
				}
			}
			else 
			{
				_player->whisper("[Server]: Invalid arguments were specified.");
			}

			return true;
		}

		if(commandArgs[0] == "kick") 
		{
			if (commandArgs.size() > 1)
			{
				std::string name = commandArgs[1];

				auto& cHandler = g_globals.connectionHandler;

				auto& clientHandles = g_globals.connectionHandler->get_client_handles();
				
				for(uint64_t clientHandle : clientHandles) 
				{
					auto optEntt = g_globals.entityHandler->get_entity(clientHandle);

					std::shared_ptr<Player> target = std::static_pointer_cast<Player>(optEntt.value());
						
					if (target->name == name)
					{
						_player->whisper("[Server]: Succesfully disconnected: " + target->name + '.');
						cHandler->disconnect_client(static_cast<enet_uint32>(clientHandle));
						return true;
					}
				}
			}

			_player->whisper("[Server]: The player you tried to kick does not exist.");
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
