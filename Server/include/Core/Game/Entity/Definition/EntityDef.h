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
	uint32_t             tickCounter = 0;
	DM::Utils::UUID      uuid        = -1;

	/// <summary>
	/// Non overridable fixed update that happens every cycle.
	/// </summary>
	void update();

	/// <summary>
	/// Internal game cycle for Entities's
	/// </summary>
	virtual void tick();

	/// <summary>
	/// Make the NPC aggressive to the following target.
	/// </summary>
	virtual void set_target(std::shared_ptr<Entity> _entity);

	/// <summary>
	/// Returns the attack range of the entity.
	/// </summary>
	virtual int get_attack_range();
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
	uint8_t wanderingDistance = 3;

	/// <summary>
	/// How far the entity can max follow someone
	/// </summary>
	uint8_t maxWanderingDistance = 5;

	/// <summary>
	/// The attack range of this NPC.
	/// </summary>
	uint8_t attackRange = 1;

	/// <summary>
	/// Tick counter for the NPC to perform certain actions.
	/// </summary>
	uint32_t tickCounter = 0;

	/// <summary>
	/// Whether this NPC is aggressive to players.
	/// </summary>
	bool bIsAgressive = false;

	/// <summary>
	/// Internal game cycle for NPC's
	/// </summary>
	virtual void tick() override;

	/// <summary>
	/// Returns the attack range
	/// </summary>
	virtual int get_attack_range() override;

	/// <summary>
	/// Make the NPC aggressive to the following target.
	/// </summary>
	virtual void set_target(std::shared_ptr<Entity> _entity) override;

private:
	std::shared_ptr<Entity> m_target;
	Utilities::ivec2        m_targetPos = Utilities::ivec2(0, 0);
	bool                    m_bIsMoving = false;
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
			data.skills[e_skills::HITPOINTS].level = 5000;
			data.skills[e_skills::ATTACK]   .level = 5;
			data.skills[e_skills::STRENGTH] .level = 5;
			data.skills[e_skills::DEFENCE]  .level = 0;
			data.wanderingDistance = 3;
		}
		break;

		default:
		{
			data.skills[e_skills::HITPOINTS].level = 100;
		}
		break;
	}

	/// <summary>
	/// Make the initial boosted level equal to the actual level of the skill.
	/// </summary>
	for(uint8_t i = 0; i < SKILL_COUNT; i++)
	{
		data.skills[(e_skills)i].levelboosted = data.skills[(e_skills)i].level;
	}

	return data;
}