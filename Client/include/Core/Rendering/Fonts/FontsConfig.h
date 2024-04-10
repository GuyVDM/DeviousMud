#pragma once
#include <cstdint>

#include <unordered_map>

#include <string>

enum class e_FontType : uint8_t
{
	RUNESCAPE_UF = 0x00
};

struct Fonts 
{
	using FontMapType = std::unordered_map<e_FontType, std::string>;

	static const FontMapType FontMap()
	{
		return 		
		{
			{ e_FontType::RUNESCAPE_UF, "runescape_uf.ttf" }
		};
	}
};