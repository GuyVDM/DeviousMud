#pragma once
#include "Shared/Game/Skill.hpp"

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include <map>

#include <array>

class Entity
{
public:
	DM::SKILLS::SkillMap skills;
	Utilities::ivec2     position;
};

class Player : public Entity
{
public:
	std::string name = "";
};

class NPC : public Entity
{
public:
	/// <summary>
	/// To indentify this entity.
	/// </summary>
	DM::Utils::UUID uuid = -1;

	/// <summary>
	/// The spawn position of this character.
	/// </summary>
	Utilities::ivec2 startingPosition = Utilities::ivec2(0, 0);

	/// <summary>
	/// What properties this npc has.
	/// </summary>
	uint8_t npcId = 1;

	/// <summary>
	/// How far the entity can randomly walk from the start.
	/// </summary>
	uint8_t wanderingDistance = 10;

	/// <summary>
	/// Whether this NPC is aggressive to players.
	/// </summary>
	bool bIsAgressive = false;

	void tick();

private:
	Utilities::ivec2 m_targetPos = Utilities::ivec2(0, 0);
	bool m_bIsMoving = false;
};

static const NPC get_entity_data(const uint8_t _id)
{
	using namespace DM::SKILLS;

	NPC data;
	data.npcId = _id;

	switch(_id) 
	{
		case 1:
		{
			data.skills[e_skills::HITPOINTS].level = 5;
			data.skills[e_skills::ATTACK].level    = 5;
			data.skills[e_skills::STRENGTH].level  = 5;
			data.skills[e_skills::DEFENCE].level   = 0;
			data.wanderingDistance = 3;
		}
		break;

		default:
		{
			data.skills[e_skills::HITPOINTS].level = 10;
		}
		break;
	}

	return data;
}