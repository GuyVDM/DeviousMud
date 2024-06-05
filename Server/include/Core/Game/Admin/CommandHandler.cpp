#include "precomp.h"

#include "Core/Game/Admin/CommandHandler.h"

#include "Core/Game/Entity/Definition/EntityDef.h"

bool CommandHandler::try_handle_as_command(std::shared_ptr<Player> _player, const std::string& _string)
{
	const static std::string commandPrefix = "::";

	const bool bIsCommand = starts_with(_string, commandPrefix);

	if(bIsCommand) 
	{
		const std::string command = _string.substr(commandPrefix.length());

		_player->tell("[Server] Command was not recognized.");
	}

	return bIsCommand;
}

bool CommandHandler::starts_with(const std::string& _str, const std::string& _prefix)
{
	return _str.find(_prefix) == 0; // Returns true if the prefix is found at position 0
}
