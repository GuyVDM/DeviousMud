#pragma once
#include "Core/Events/Clickable/Clickable.h"

#include "Core/NPC/NPCDef.h"

#include "Core/Entity/Simulation/SimPosition.h"

class EntityHandler;

enum class e_InteractionMode 
{
	STATE_INTERACTABLE = 0x00,
	STATE_MASKED       = 0x01
};

class WorldEntity : public Clickable 
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

	void move_to(const Utilities::ivec2 _pos);

	const NPCDef&         get_definition() const;

	const SimPosition&    get_simulated_data() const;

	const Utilities::vec2 get_position() const;

	virtual bool handle_event(const SDL_Event* _event) override;

public:
	virtual ~WorldEntity();

private:
	WorldEntity(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);

private:
	void update();

private:
	DM::Utils::UUID   m_entityUUID = -1;
	e_InteractionMode m_eInteractionMode = e_InteractionMode::STATE_INTERACTABLE;
	NPCDef            m_NPCDefinition;
	SimPosition       m_simPos;
	DM::Utils::UUID   m_updateListenerUUID;
};  

using RefEntity = std::shared_ptr<WorldEntity>;

