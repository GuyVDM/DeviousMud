#include "precomp.h"

#include "Core/Entity/WorldEntity/WorldEntity.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Animation/Animator/Animator.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Rendering/Camera/Camera.h"

RefEntity WorldEntity::create_entity(uint8_t _npcId, Utilities::ivec2 _pos, DM::Utils::UUID _uuid)
{
    const NPCDef npcDef = get_npc_definition(_npcId);

    WorldEntity* entity = new WorldEntity
    (
        Utilities::to_vec2(_pos) * Graphics::Renderer::GRID_CELL_PX_SIZE,
        npcDef.size              * Graphics::Renderer::GRID_CELL_PX_SIZE,
        g_globals.renderer.lock()->get_sprite(npcDef.sprite)
    );  
    
    //Set the simulating start of the position equal to the start location.
    entity->m_simPos.m_currentPos = Utilities::to_vec2(_pos);
    entity->m_NPCDefinition = npcDef;
    entity->m_entityUUID = _uuid;

    Graphics::Animation::Animator::set_default_animation(entity->m_sprite, npcDef.idleAnim, 8.0f);

    return std::shared_ptr<WorldEntity>(entity);
}

WorldEntity::WorldEntity(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite) : Clickable(_pos, _size, _sprite)
{
    m_updateListenerUUID = g_globals.entityHandler.lock()->on_entity_update.add_listener
    (
        std::bind(&WorldEntity::update, this)
    );

    // Bind callback to the end of the walking cycle to default animation.
    {
        auto stop_curr_anim_func = [this]()
        {
            Graphics::Animation::Animator::stop_current_animation(m_sprite);
        };  

        m_simPos.on_reached_dest.add_listener(stop_curr_anim_func);
    }
}

WorldEntity::~WorldEntity()
{
    g_globals.entityHandler.lock()->on_entity_update.remove_listener
    (
        m_updateListenerUUID
    );
}

const NPCDef& WorldEntity::get_definition() const
{
    return m_NPCDefinition;
}

const SimPosition& WorldEntity::get_simulated_data() const
{
    return m_simPos;
}

void WorldEntity::set_interaction_mode(e_InteractionMode _interactionMode)
{
    m_eInteractionMode = _interactionMode;
}

void WorldEntity::move_to(const Utilities::ivec2 _pos)
{
    // We only want to move if the target is NOT the same as the current one.
    if (_pos != Utilities::to_ivec2(get_position())) 
    {
        if (!m_simPos.is_dirty()) //Play animation only if it just started walking.
        {
            Graphics::Animation::Animator::play_animation(m_sprite, m_NPCDefinition.walkingAnim, true, 10.0f);
        }

        m_sprite.bIsFlipped = _pos.x < get_position().x;
        m_simPos.set_target(Utilities::to_vec2(_pos));
        set_position(Utilities::to_vec2(_pos));
    }
}

const Utilities::vec2 WorldEntity::get_position() const
{
    return m_simPos.get_position();
}

bool WorldEntity::handle_event(const SDL_Event* _event)
{
    if (is_hovered())
    {
        switch (_event->type)
        {
        case SDL_MOUSEBUTTONDOWN:
        {
            if (_event->button.button == SDL_BUTTON_RIGHT && m_eInteractionMode == e_InteractionMode::STATE_INTERACTABLE)
            {
                const auto follow_entity = [this]()
                {
                    Packets::s_EntityFollow packet;
                    packet.interpreter = e_PacketInterpreter::PACKET_FOLLOW_ENTITY;
                    packet.action = e_Action::SOFT_ACTION;
                    packet.entityId = m_entityUUID;

                    auto packetHandler = g_globals.packetHandler.lock();
                    packetHandler->send_packet<Packets::s_EntityFollow>(&packet, 0, ENET_PACKET_FLAG_RELIABLE);
                };

                OptionArgs m_textArgs;
                m_textArgs.actionStr = "Follow";
                m_textArgs.subjectStr = "Entity";
                m_textArgs.actionCol = { 255, 255, 0, 255 };
                m_textArgs.function = std::bind(follow_entity);
                OptionsTab::add_option(m_textArgs);
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
    if(m_simPos.is_dirty()) 
    {
        m_simPos.update();
        return;
    }
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

