#pragma once
#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/vec2.hpp"

/// <summary>
/// Simulates the path that is taken by an actor based on server input.
/// </summary>
class SimPosition
{
public:
	const Utilities::vec2 get_position() const;

public:
	~SimPosition() = default;

	explicit SimPosition();
	explicit SimPosition(const Utilities::_vec2& _startPos);

private:
	EventListener<void> on_reached_dest;

	const bool& is_dirty() const;

	void update();

	void set_target(const Utilities::vec2 _details);

	void set_current_position(const Utilities::vec2 _pos);

private:
	Utilities::vec2 m_startPos;
	Utilities::vec2 m_currentPos;
	Utilities::vec2 m_endPos;

	std::vector<Utilities::ivec2> m_path;

	bool  m_bIsDirty = false;
	float m_elapsedTime = 0.0f;

	/// <summary>
	/// How many seconds should pass within the animation relative to a real second.
	/// </summary>
	const float RATE = 1.7f;

	friend class WorldEntity;
};