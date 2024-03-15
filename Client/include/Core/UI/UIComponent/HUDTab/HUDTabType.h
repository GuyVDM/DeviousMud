#pragma once
typedef unsigned char uint8_t;

enum class e_TabType :  uint8_t
{
	NONE      = 0x00,
	INVENTORY = 0x01,
	QUESTS    = 0x02,
	SKILLS    = 0x03,
	EQUIPMENT = 0x04,
	PRAYER    = 0x05,

	PLACEHOLDER_06 = 0x06,
	PLACEHOLDER_07 = 0x07,
	PLACEHOLDER_08 = 0x08,
	PLACEHOLDER_09 = 0x09,
	PLACEHOLDER_10 = 0x0A
};