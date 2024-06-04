#include "precomp.h"

#include "Core/Global/C_Globals.h"

#include "Core/UI/UIComponent/WorldText/WorldText.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Util/TimerHandler.h"

#include "Core/Application/Config/Config.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Entity/EntityHandler.h"

void WorldText::set_follow_target(DM::Utils::UUID _targetId)
{
	auto optEntt = g_globals.entityHandler.lock()->get_entity(_targetId);

	if (optEntt.has_value())
	{
		RefEntity entt = optEntt.value();
		m_target = entt;
		return;
	}
}

void WorldText::set_text(std::string _text)
{
	//*-------------------------------------------------
	// Reuse this textcomponent if it's still available.
	//*
	if(get_child_count() > 0) 
	{
		m_children.clear();
		m_elapsedTime = 0.0f;
	}

	TextArgs textArgs;
	{
		textArgs.color = { 255, 255, 0, 255 };
		textArgs.font = e_FontType::RUNESCAPE_UF;
		textArgs.size = 24;
		textArgs.bDropShadow = true;
	}

	// Create hitamount text
	{
		auto textComponent = Graphics::TextComponent::create_text
		(
			_text,
			get_position(),
			textArgs
		);

		add_child(textComponent);
	}
}

void WorldText::update()
{
	m_elapsedTime = CLAMP(m_elapsedTime + DM::CLIENT::Config::get_deltaTime(), 0.0f, m_displayDuration);

	// Movement of the hitsplash
	{
		auto& textComponent = m_children[0];

		if (!m_target.expired())
		{
			auto target = m_target.lock();

			Utilities::vec2 pos = target->get_simulated_data().get_position() - Utilities::vec2(0.25f);
			pos.x += (target->get_size().x        / 2.0f) / static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE);
			pos.x -= (textComponent->get_size().x / 2.0f) / static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE);
			pos.y -= 50.0f / static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE);
			textComponent->set_position(pos);
		}
		else 
		{
			//Immidiately unparent.
			m_elapsedTime = m_displayDuration;
		}
	}

	// Delete hitsplash after a while
	if (m_elapsedTime == m_displayDuration)
	{
		unparent();
	}
}