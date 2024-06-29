#include "precomp.h"

#include "Core/Entity/WorldEntity/WorldEntity.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Util/TimerHandler.h"

#include "Core/Rendering/Camera/Camera.h"

#include "Core/UI/UIComponent/Canvas/Canvas.h"

#include "Core/UI/UIComponent/Hitsplat/Hitsplat.h"

#include "Core/UI/UIComponent/WorldText/WorldText.h"

#include "Core/UI/UIComponent/Healthbar/Healthbar.h"

#include "Core/Entity/WorldEntity/States/WorldEntityStates.h"

RefEntity WorldEntity::create_entity(uint8_t _npcId, Utilities::ivec2 _pos, DM::Utils::UUID _uuid)
{
    //*-------------------------------------------------------
    // Create the entity and register a shared pointer for it.
    //*
    const NPCDef npcDef = get_npc_definition(_npcId);
    WorldEntity* entity = new WorldEntity
    (
        Utilities::to_vec2(_pos)                        * static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE),
        Utilities::vec2(static_cast<float>(npcDef.size) * static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE)),
        g_globals.renderer.lock()->get_sprite(npcDef.sprite)
    );  

    std::shared_ptr<WorldEntity> shrdEntity = std::shared_ptr<WorldEntity>(entity);

    //*----------------
    // Set entity name.
    //*
    {
        shrdEntity->set_name(npcDef.name);
    }
    
    //*---------------------------------------------
    // Register UI elements and entity specific data.
    //*
    {
        const int8_t playerUI_zOrder = 5;
        entity->m_simPos.m_currentPos = Utilities::to_vec2(_pos);
        entity->m_npcDefinition = npcDef;
        entity->m_entityUUID = _uuid;
        entity->m_canvas = Canvas::create_canvas();
        entity->m_canvas->set_render_mode(UIComponent::e_RenderMode::WORLDSPACE);
        entity->m_canvas->set_z_order(playerUI_zOrder);
    }
    
    //*-----------------------------------
    //  Register healthbar for the entity.
    //*
    {
        auto healthbar = Healthbar::create_hp_bar(Utilities::vec2(50.0f, 7.0f), shrdEntity);
        entity->m_canvas->add_child(healthbar);
    }

    //*------------------------------------------
    // Set default idle animation for the entity.
    //*
    Graphics::Animation::Animator::set_default_animation(entity->m_sprite, npcDef.idleAnim, 8.0f);

    //*------------------------------------------
    // Declare all states for the world entity.
    //*
    using namespace DM::State;

    entity->m_states[e_EntityState::STATE_IDLE]      = std::make_shared<EntityIdleState>(shrdEntity);
    entity->m_states[e_EntityState::STATE_WALKING]   = std::make_shared<EntityWalkingState>(shrdEntity);
    entity->m_states[e_EntityState::STATE_ATTACKING] = std::make_shared<EntityAttackingState>(shrdEntity);
    entity->m_states[e_EntityState::STATE_DEAD]      = std::make_shared<EntityDeathState>(shrdEntity);

    entity->m_currentState = entity->m_states[e_EntityState::STATE_IDLE];
    entity->m_currentState->on_state_start();

    return shrdEntity;
}

WorldEntity::WorldEntity(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite) : Clickable(_pos, _size, _sprite)
{
}

WorldEntity::~WorldEntity()
{
}

const NPCDef& WorldEntity::get_definition() const
{
    return m_npcDefinition;
}

const SimPosition& WorldEntity::get_simulated_data() const
{
    return m_simPos;
}

void WorldEntity::set_interaction_mode(e_InteractionMode _interactionMode)
{
    m_eInteractionMode = _interactionMode;
}

void WorldEntity::say(std::string _text)
{
    std::shared_ptr<WorldText> text;

    //*---------------------------------------------------------------------
    // Try grab existing text component if its there, if not, make a new one.
    //*
    {
        auto optText = m_canvas->get_component_in_children<WorldText>();
        if (optText.has_value())
        {
            text = optText.value();
        }
        else
        {
            text = UIComponent::create_component<WorldText>
            (
                get_position() + Utilities::vec2(get_size().x / 2.0f, 0.0f),
                Utilities::vec2(1.0f), 
                Graphics::SpriteType::NONE, 
                false
            );

            m_canvas->add_child(text);
        }
    }

    //*------------------------------------------------
    // Register the TextComponent and set the contents.
    //*
    {
        text->set_text(_text);

        text->set_follow_target(m_entityUUID);
    }
}

void WorldEntity::teleport_to(Utilities::vec2 _destination)
{
    m_simPos.m_bIsDirty = false;
    m_simPos.m_currentPos = _destination;
}

void WorldEntity::die()
{
    m_bIsDead = true;
    Graphics::Animation::Animator::play_animation_oneshot(m_sprite, m_npcDefinition.deathAnim, 8.0f);
}

const bool WorldEntity::in_combat() const
{
    return m_bInCombat;
}

void WorldEntity::turn_to(DM::Utils::UUID _entityId)
{
    std::optional<RefEntity> enttOpt = g_globals.entityHandler.lock()->get_entity(_entityId);

    if(RefEntity& entt = enttOpt.value(); enttOpt.has_value()) 
    {
        Utilities::vec2 enttPos = entt->get_position();

        m_sprite.bIsFlipped = enttPos.x < get_position().x ? true : false;
    }
}

void WorldEntity::hit(int32_t _hitAmount)
{
    //*-----------------
    // Add the hitsplat.
    //*
    {
        Utilities::vec2 simpos = m_simPos.get_position();

        std::shared_ptr<Hitsplat> hitsplat = UIComponent::create_component<Hitsplat>
        (
            m_simPos.get_position(),
            Utilities::vec2(40.0f, 40.0f),
            Graphics::SpriteType::HITSPLAT
        );

        hitsplat->set_hit_amount(_hitAmount);
        hitsplat->set_follow_target(m_entityUUID);
        m_canvas->add_child(hitsplat);
    }

    //*------------------------
    // Register in combat timer.
    //*
    {
        m_bInCombat = true;
        
        TimerArgs args;
        std::weak_ptr<WorldEntity> selfPtr = shared_from_this();

        args.duration = 5.0f;
        args.function = [selfPtr]()
        {
            if (std::shared_ptr<WorldEntity> entity = selfPtr.lock(); !selfPtr.expired())
            {
                entity->m_bInCombat = false;
            }
        };

        g_globals.timerHandler.lock()->add_timer
        (
            m_entityUUID,
            args
        );
    }
}

void WorldEntity::attack()
{
    set_state(e_EntityState::STATE_ATTACKING);
}

void WorldEntity::update_skill(uint8_t _skillType, int32_t _level, int32_t _levelBoosted)
{
    DM::SKILLS::e_skills type = static_cast<DM::SKILLS::e_skills>(_skillType);
    DM::SKILLS::Skill* skill = &m_skills[type];
    skill->level = _level;
    skill->levelboosted = _levelBoosted;
}

DM::SKILLS::SkillMap& WorldEntity::get_skills()
{
    return m_skills;
}

void WorldEntity::move_to(const Utilities::ivec2 _pos)
{
    //*----------------------------------------------------------------------
    // We only want to move if the target is NOT the same as the current one.
    //
    if (_pos == Utilities::to_ivec2(m_simPos.m_endPos))
    {
        return;
    }

    using namespace Graphics::Animation;

    m_simPos.set_target(Utilities::to_vec2(_pos));
    m_sprite.bIsFlipped = _pos.x < get_position().x;
    set_position(Utilities::to_vec2(_pos));
}

const Utilities::vec2 WorldEntity::get_position() const
{
    return m_simPos.get_position();
}

const bool WorldEntity::is_visible() const
{
    return !m_bShouldHide;
}

void WorldEntity::set_visibility(bool _bShouldHide)
{
    m_bShouldHide = _bShouldHide;
}

bool WorldEntity::handle_event(const SDL_Event* _event)
{
    if (m_bIsDead)
        return false;

    if (is_hovered())
    {
        switch (_event->type)
        {
        case SDL_MOUSEBUTTONDOWN:
        {
            if (_event->button.button == SDL_BUTTON_RIGHT && m_eInteractionMode == e_InteractionMode::STATE_INTERACTABLE)
            {
                //Attack option
                {
                    const auto attack_entity = [this]()
                    {
                        Packets::s_ActionPacket packet;
                        packet.interpreter = e_PacketInterpreter::PACKET_ENGAGE_ENTITY;
                        packet.action      = e_Action::MEDIUM_ACTION;
                        packet.entityId    = m_entityUUID;

                        auto packetHandler = g_globals.packetHandler.lock();
                        packetHandler->send_packet<Packets::s_ActionPacket>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);
                    };

                    OptionArgs option;
                    option.actionStr = "Attack";
                    option.subjectStr = m_name;
                    option.actionCol = { 255, 255, 0, 255 };
                    option.function = std::bind(attack_entity);
                    OptionsTab::add_option(option);
                }

                //Follow option.
                {
                    const auto follow_entity = [this]()
                    {
                        Packets::s_ActionPacket packet;
                        packet.interpreter = e_PacketInterpreter::PACKET_FOLLOW_ENTITY;
                        packet.action = e_Action::SOFT_ACTION;
                        packet.entityId = m_entityUUID;

                        auto packetHandler = g_globals.packetHandler.lock();
                        packetHandler->send_packet<Packets::s_ActionPacket>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);
                    };

                    OptionArgs option;
                    option.actionStr = "Follow";
                    option.subjectStr = m_name;
                    option.actionCol = { 255, 255, 0, 255 };
                    option.function = std::bind(follow_entity);
                    OptionsTab::add_option(option);
                }
            }
        }
        break;
        }
    }

    Clickable::handle_event(_event);

    return false;
}

void WorldEntity::update()
{
    m_canvas->recursiveUpdateCleanup();

    m_currentState->on_state_update();

    if (m_bShouldHide)
        return;

    m_canvas->render();
}

void WorldEntity::set_name(const std::string& _name)
{
    m_name = _name;
}

const std::string& WorldEntity::get_name() const
{
    return m_name;
}

void WorldEntity::respawn() 
{
    //*-----------------------------------
    // Make the entity interactable again.
    //*
    {
        m_bIsDead = false;
    }
}

void WorldEntity::set_state(const e_EntityState& _state)
{
    m_currentState->on_state_end();
    m_currentState = m_states[_state];
    m_currentState->on_state_start();
}

const bool WorldEntity::overlaps_rect(const int& _x, const int& _y) const
{
    const Utilities::ivec2 camPos = Utilities::to_ivec2(g_globals.renderer.lock()->get_camera()->get_position());

    Utilities::ivec2 viewportSize;
    g_globals.renderer.lock()->get_viewport_size(&viewportSize.x, &viewportSize.y);
    viewportSize.x /= 2;
    viewportSize.y /= 2;

    // Calculate sprite's screen coordinates
    const Utilities::ivec2 halfExtends = Utilities::to_ivec2(get_size() / 2.0f);

    const Utilities::ivec2 transformedPos = Utilities::to_ivec2(m_simPos.get_position() * Graphics::Renderer::GRID_CELL_PX_SIZE) - camPos
                                          + (viewportSize + halfExtends);

    // Calculate sprite's bounding box
    const int32_t left   = transformedPos.x - halfExtends.x;
    const int32_t right  = transformedPos.x + halfExtends.x;
    const int32_t top    = transformedPos.y - halfExtends.y;
    const int32_t bottom = transformedPos.y + halfExtends.y;

    return (_x > left && _x < right && _y > top && _y < bottom);
}