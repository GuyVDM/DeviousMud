#include "precomp.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/Hitsplat/Hitsplat.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Util/TimerHandler.h"

#include "Core/Application/Config/Config.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Entity/EntityHandler.h"

void Hitsplat::set_follow_target(DM::Utils::UUID _targetId)
{
	auto optEntt = g_globals.entityHandler.lock()->get_entity(_targetId);

	if(RefEntity& entt = optEntt.value(); optEntt.has_value()) 
	{
		m_target = entt;
		return;
	}
}

void Hitsplat::set_hit_amount(int32_t _hitAmount)
{
	Utilities::vec2 pos = get_position();

	TextArgs textArgs;
	{
		textArgs.color = { 255, 255, 255, 255 };
		textArgs.font = e_FontType::RUNESCAPE_UF;
		textArgs.size = 20;
		textArgs.bDropShadow = true;
	}

	// Create hitamount text
	{
		auto textComponent = Graphics::TextComponent::create_text
		(
			std::to_string(_hitAmount),
			get_position(),
			textArgs
		);
		add_child(textComponent);

		Utilities::vec2 textSize = textComponent->get_size();

		Utilities::vec2 offsetToCenter = (get_size() - textSize) / 2.0f;

		Utilities::vec2 manualOffset = offsetToCenter + Utilities::vec2(0.0f, 0.0f);

		textComponent->set_position(pos + (manualOffset / (float)Graphics::Renderer::GRID_CELL_PX_SIZE));
	}
}

void Hitsplat::update()
{
	const Uint8 alpha = static_cast<Uint8>(roundf(255.0f * (1.0f - m_elapsedTime / m_fadeDuration)));
	
	m_elapsedTime = CLAMP(m_elapsedTime + DM::CLIENT::Config::get_deltaTime(), 0.0f, m_fadeDuration);

	m_sprite.color.a = alpha;

	for(auto& uiComponent : m_children) 
	{
		uiComponent->get_sprite().color.a = alpha;
	}

	// Movement of the hitsplash
	{
		Utilities::vec2 pos = {};
		{
			if (!m_target.expired())
			{
				pos = m_target.lock()->get_simulated_data().get_position();
			}
			else
			{
				pos = get_position();
			}
		}

		Utilities::vec2 velocity = Utilities::vec2(0.0f, -1.0f) * m_elapsedTime;

		set_position(pos + velocity);
	}

	Utilities::vec2 pos = get_position();

	// Delete hitsplash after a while
	if(m_elapsedTime == m_fadeDuration)
	{
		unparent();
	}
}
