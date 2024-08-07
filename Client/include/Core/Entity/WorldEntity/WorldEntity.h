#pragma once
#include "Core/Events/Clickable/Clickable.h"


#include "Core/Entity/Simulation/SimPosition.h"

#include "Shared/Game/NPCDef.h"

#include "Shared/Game/Skill.hpp"

class EntityHandler;
class Canvas;

namespace DM
{
	namespace State
	{
		template<class T>
		class State;
	}
}

template<class T>
using RefState = std::shared_ptr<DM::State::State<T>>;

enum class e_InteractionMode 
{
	STATE_INTERACTABLE = 0x00,
	STATE_MASKED       = 0x01
};

enum class e_EntityState 
{
	STATE_IDLE      = 0x00,
	STATE_WALKING   = 0x01,
	STATE_ATTACKING = 0x02,
	STATE_DEAD      = 0x03
};

class WorldEntity : public Clickable, public std::enable_shared_from_this<WorldEntity>
{
public:
	/// <summary>
	/// 0 is for players.
	///
	/// Creates a new World Entity.
	/// </summary>
	/// <param name="_npcId"> The Id to grab the NPC Definition with. </param>
	/// <param name="_pos"> What world coordinates it should spawn at. </param>
	/// <param name="_size"> How big this entity is in tiles. </param>
	/// <returns></returns>
	static std::shared_ptr<WorldEntity> create_entity(const uint8_t _npcId, const Utilities::ivec2 _pos, DM::Utils::UUID _uuid);

public:
	void set_interaction_mode(e_InteractionMode _interactionMode);

	/// <summary>
	/// Prints text above the entity its head temporarily.
	/// </summary>
	/// <param name="_text"></param>
	void say(std::string _text);

	/// <summary>
	/// To make the entity instantly teleport.
	/// </summary>
	/// <param name="_destination"></param>
	void teleport_to(Utilities::vec2 _destination);

	/// <summary>
	/// Plays the death animation;
	/// </summary>
	void die();

	/// <summary>
	/// Makes the entity interactable and visible again.
	/// </summary>
	void respawn();

	/// <summary>
	/// Update the statemachine and set a new state.
	/// </summary>
	void set_state(const e_EntityState& _state);

	/// <summary>
	/// Whether this Entity is in combat.
	/// </summary>
	/// <returns></returns>
	const bool in_combat() const;

	/// <summary>
	/// Make entity face towards another entity horizontally.
	/// </summary>
	/// <param name="_entityId"></param>
	void turn_to(DM::Utils::UUID _entityId);


	/// <summary>
	/// TODO: Make it summon a hitsplat.
	/// </summary>
	void hit(int32_t _hitAmount);


	/// <summary>
	/// Triggers visuals for the attack.
	/// </summary>
	void attack();

	
	/// <summary>
	/// Moves the entity to the specified coordinates on screen.
	/// </summary>
	/// <param name="_pos"></param>
	void move_to(const Utilities::ivec2 _pos);


	/// <summary>
	/// Returns the definition of how the Entity should get rendered.
	/// </summary>
	/// <returns></returns>
	const NPCDef& get_definition() const;


	/// <summary>
	/// Returns the component that takes care of simulating server based movement.
	/// </summary>
	/// <returns></returns>
	const SimPosition& get_simulated_data() const;


	/// <summary>
	/// Returns the matching server position.
	/// </summary>
	/// <returns></returns>
	const Utilities::vec2 get_position() const;


	/// <summary>
	/// Whether this entity should get rendered.
	/// </summary>
	/// <returns></returns>
	const bool is_visible() const;


	/// <summary>
	/// Whether this entity should get hidden.
	/// </summary>
	void set_visibility(bool _bShouldHide);


	/// <summary>
	/// Updates a specified skill for visual purposes, usually to represent server side variables..
	/// </summary>
	/// <param name="_skillType"></param>
	/// <param name="_level"></param>
	/// <param name="_levelBoosted"></param>
	void update_skill(uint8_t _skillType, int32_t _level, int32_t _levelBoosted);


	/// <summary>
	/// Called every frame.
	/// </summary>
	virtual void update();


	/// <summary>
	/// Set the name of the entity.
	/// </summary>
	/// <param name="_name"></param>
	void set_name(const std::string& _name);


	/// <summary>
	/// Get the name of the entity.
	/// </summary>
	/// <returns></returns>
	const std::string& get_name() const;


	/// <summary>
	/// Returns the map of skills of the specified entity.
	/// </summary>
	/// <returns></returns>
	DM::SKILLS::SkillMap& get_skills();

public:
	virtual bool handle_event(const SDL_Event* _event) override;

	virtual ~WorldEntity();

private:
	WorldEntity(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);
	
	virtual const bool overlaps_rect(const int& _x, const int& _y) const override;

	friend std::shared_ptr<WorldEntity>;
	friend class EntityWalkingState;
	friend class EntityDeathState;
	friend class EntityIdleState;
	friend class EntityAttackingState;

private:	
	std::string             m_name = "null";
	
	DM::Utils::UUID         m_entityUUID = -1;

	bool                    m_bShouldHide = false;

	bool                    m_bIsDead = false;
	
	bool                    m_bInCombat = false;

	DM::SKILLS::SkillMap    m_skills;

	NPCDef                  m_npcDefinition;

	SimPosition             m_simPos;

	e_InteractionMode       m_eInteractionMode = e_InteractionMode::STATE_INTERACTABLE;

	DM::Utils::UUID         m_updateListenerUUID;

	std::shared_ptr<Canvas> m_canvas;

	RefState<WorldEntity>   m_currentState;

	std::map<e_EntityState, RefState<WorldEntity>> m_states;
};  

using RefEntity = std::shared_ptr<WorldEntity>;




