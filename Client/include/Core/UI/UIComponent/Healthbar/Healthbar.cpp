#include "precomp.h"

#include "Core/UI/UIComponent/Healthbar/Healthbar.h"

#include "Core/Entity/WorldEntity/WorldEntity.h"

#include "Core/Entity/EntityHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/Rendering/Renderer.h"

#include "Shared/Game/Skill.hpp"

std::shared_ptr<Healthbar> Healthbar::create_hp_bar(Utilities::vec2 _size, std::shared_ptr<WorldEntity> _owner)
{
    auto healthbar = UIComponent::create_component<Healthbar>
        (
            Utilities::vec2(0.0f),
            _size,
            Graphics::SpriteType::NONE,
            false
        );

    healthbar->m_owner = _owner;
    healthbar->m_assetName = "Healthbar";
    return healthbar;
}

void Healthbar::init()
{
    on_render_call.add_listener
    (
        std::bind(&Healthbar::render_healthbar, this, std::placeholders::_1)
    );
}

void Healthbar::render_healthbar(std::shared_ptr<Graphics::Renderer> _renderer)
{
    if (const auto& owner = m_owner.lock(); !m_owner.expired())
    {
        //Only render healthbars for npc's in combat.
        if (owner->in_combat())
        {
            const Utilities::vec2 offset = Utilities::vec2(-0.1f, -0.7f);
            const Utilities::vec2 enttPosition = owner->get_simulated_data().get_position() + offset;

            //Draw fillAmount of healthbar.
            {
                const DM::SKILLS::Skill* hitpoints = &owner->get_skills()[DM::SKILLS::e_skills::HITPOINTS];

                const float hpRemainingNorm = CLAMP((float)hitpoints->levelboosted / (float)hitpoints->level, 0.0f, 1.0f);

                Utilities::vec2 fillSize = get_size();
                fillSize.x = get_size().x * hpRemainingNorm;

                //Draw backdrop of healthbar
                _renderer->draw_rect(enttPosition, get_size(), {255, 0, 0, 255}, m_sprite.zRenderPriority, false);
                
                //Draw fill of healthbar
                _renderer->draw_rect(enttPosition, fillSize, { 0, 255, 0, 255 }, m_sprite.zRenderPriority, false);
            }
        }
    }
}
