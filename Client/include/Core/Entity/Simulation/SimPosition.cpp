#include "precomp.h"

#include "Core/Entity/Simulation/SimPosition.h"

#include "Core/Application/Config/Config.h"

#include "Shared/Navigation/AStar.hpp"

void SimPosition::set_target(const Utilities::vec2 _target)
{
	//*----------------------------------------------------------------------
	// Skip setting new target if the target was the same as the previous one.
	//*
	if (Utilities::to_ivec2(_target) == Utilities::to_ivec2(m_endPos))
		return;

	m_bIsDirty = true;
	m_elapsedTime = 0.0f;

	//*-----------------
	//  Update positions
	//*
	{
		m_startPos = m_currentPos;
		m_endPos = _target;

		if (Utilities::to_ivec2(m_startPos) != Utilities::to_ivec2(m_endPos))
		{
			m_path = AStar::find_path
			(
				Utilities::to_ivec2(m_startPos),
				Utilities::to_ivec2(m_endPos)
			);
		}
		else 
		{
			m_path = std::vector<Utilities::ivec2>{ Utilities::to_ivec2(m_endPos) };
		}
	}
}

void SimPosition::set_current_position(const Utilities::vec2 _pos)
{
	m_currentPos = _pos;
}

void SimPosition::update()
{
	m_elapsedTime += RATE * DM::CLIENT::Config::get_deltaTime();

	const static float MAX_TIMELINE     = 1.0f;
	const float        DURATION_TO_TILE = 1.0f / m_path.size();

	float              timeline = CLAMP(m_elapsedTime, 0.0f, 1.0f);

	//*
	// Calculate which tile we're walking towards.
	//*
	const int32_t targetTileIndex = static_cast<uint32_t>
	(
		CLAMP(ceilf(timeline / (1.0f / m_path.size())) - 1.0f, 0, INT_FAST32_MAX)
	);

	//*
	// Calculate the current position.
	//*
	{
		//Check whether we already passed by one of the tiles, if not, use the start position.
		const Utilities::vec2 START_TILE = targetTileIndex == 0 ? m_startPos : Utilities::to_vec2(m_path[targetTileIndex - 1]);

		const Utilities::vec2 END_TILE = Utilities::to_vec2(m_path[targetTileIndex]);

		float SMOOTHED_TIMELINE = DMMath::smoothstep(0.0f, 1.0f, timeline); // Combine both smoothsteps
		
		const float LOCAL_TIMELINE = (SMOOTHED_TIMELINE - (targetTileIndex * DURATION_TO_TILE)) / DURATION_TO_TILE;


		m_currentPos = Utilities::vec2::lerp(START_TILE, END_TILE, LOCAL_TIMELINE);
	}



	if (timeline == MAX_TIMELINE)
	{
		m_bIsDirty = false;
		on_reached_dest.invoke();
	}
}

const bool& SimPosition::is_dirty() const
{
	return m_bIsDirty;
}

//TODO: Remove the offset and base it all on the way it's rendered.
const Utilities::vec2 SimPosition::get_position() const
{
	//Offset to get the player icon centric to the tile.
	const Utilities::vec2 offset = Utilities::vec2(0.25f);
	return m_currentPos + offset;
}

SimPosition::SimPosition()
{
	m_bIsDirty = false;
	m_elapsedTime = 0.0f;
	m_startPos = Utilities::vec2(0.0f);
	m_currentPos = m_startPos;
	m_endPos = m_startPos;
}

SimPosition::SimPosition(const Utilities::_vec2& _startPos)
{
	m_bIsDirty = false;
	m_elapsedTime = 0.0f;
	m_startPos = _startPos;
	m_currentPos = _startPos;
	m_endPos = _startPos;
}
