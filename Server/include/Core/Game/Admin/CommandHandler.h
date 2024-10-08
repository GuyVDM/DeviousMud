#pragma once
#include <string>

#include <memory>

class Player;

class CommandHandler 
{
public:
	static bool try_handle_as_command(std::shared_ptr<Player> _player, const std::string& _string);

private:
	static bool starts_with(const std::string& _str, const std::string& _prefix);

	static bool try_parse_as_int(const std::string& _string, int32_t& _value);
};