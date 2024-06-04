#pragma once
#include "Shared/Game/Skill.hpp"

#include "Shared/Utilities/UUID.hpp"

#include "Shared/Utilities/vec2.hpp"

#include <map>

#include <array>

namespace Server
{
	class EntityHandler;
}

class Entity
{
public:
	Utilities::ivec2     respawnLocation = Utilities::ivec2(0, 0);
	DM::SKILLS::SkillMap skills;
	Utilities::ivec2     position    = { 0 };
	uint32_t             tickCounter = 0;
	DM::Utils::UUID      uuid        = 0;

	/// <summary>
	/// Kills the entity.
	/// </summary>
	virtual void die();

	/// <summary>
	/// Non overridable fixed update that happens every cycle.
	/// </summary>
	void update();

	/// <summary>
	/// Updates the skill stats across clients.
	/// </summary>
	/// <param name="_entity"></param>
	/// <param name="_wasInstigated"></param>
	void broadcast_skill(DM::SKILLS::e_skills _skill);

	/// <summary>
	/// Teleports the entity towards these coordinates.
	/// </summary>
	/// <param name="_coords"></param>
	void teleport_to(Utilities::ivec2 _destination);

	/// <summary>
	/// Hit the entity for X amount.
	/// </summary>
	/// <param name="hitAmount"></param>
	void broadcast_hit(std::shared_ptr<Entity> _by, int32_t _hitAmount) const;

	/// <summary>
	/// Whether the entity should get hidden client side.
	/// </summary>
	void hide_entity(const bool _bShouldHide) const;

	/// <summary>
	/// Internal game cycle for Entities's
	/// </summary>
	virtual void tick();

	/// <summary>
	/// Make the NPC aggressive to the following target.
	/// </summary>
	virtual void try_set_target(std::shared_ptr<Entity> _entity, bool _bWasInstigated = false);

	/// <summary>
	/// Disengages combat with current target.
	/// </summary>
	virtual void disengage();

	/// <summary>
	/// Returns the interval the entity can attack in.
	/// </summary>
	virtual int get_attack_speed();

	/// <summary>
	/// Returns the attack range of the entity.
	/// </summary>
	virtual int get_attack_range();

	/// <summary>
	/// How long it takes for this entity to respawn.
	/// </summary>
	virtual int32_t get_respawn_timer();

	/// <summary>
	/// Logic that happens when a entity gets back into the game.
	/// </summary>
	virtual void respawn();

	/// <summary>
	/// Whether the entity is dead.
	/// </summary>
	/// <returns></returns>
	const bool is_dead() const;

protected:
	mutable bool        m_bHideEntity = false;
	bool                m_bIsDead     = false;
	int32_t             m_respawnTimerElapsed = 0;

	//How much time there's reserved before we start counting respawn timer.
	int32_t m_deathTransitionTime = 5;
};

class Player : public Entity
{
public:
	std::string name = "";

	/// <summary>
	/// Kills & prevents the player from doing any actions.
	/// </summary>
	virtual void die() override;
};

class NPC : public Entity
{
public:
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
	uint8_t maxWanderingDistance = 3;

	/// <summary>
	/// The attack range of this NPC.
	/// </summary>
	uint8_t attackRange = 1;

	/// <summary>
	/// Whether this NPC is aggressive to players.
	/// </summary>
	bool bIsAgressive = false;

	/// <summary>
	/// Internal game cycle for NPC's
	/// </summary>
	virtual void tick() override;

	/// <summary>
	/// How long it takes for the NPC to respawn.
	/// </summary>
	/// <returns></returns>
	virtual int32_t get_respawn_timer() override;

	/// <summary>
	/// Returns the attack range
	/// </summary>
	virtual int get_attack_range() override;

	/// <summary>
	/// Make the NPC aggressive to the following target.
	/// </summary>
	virtual void try_set_target(std::shared_ptr<Entity> _entity, bool _bWasInstigated) override;

	/// <summary>
	/// Sets target to null after it's been defeated.
	/// </summary>
	virtual void disengage() override;

private:
	float                   m_respawnTimer = 0.0f;
	std::weak_ptr<Entity>   m_target;
	Utilities::ivec2        m_targetPos = Utilities::ivec2(0, 0);
	bool                    m_bIsMoving = false;

	friend class Server::EntityHandler;
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