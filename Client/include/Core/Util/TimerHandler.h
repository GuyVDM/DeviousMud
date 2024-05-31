#pragma once
#include <functional>

#include "Core/Application/Config/Config.h"

#include "Shared/Utilities/UUID.hpp"

struct TimerArgs
{
	float duration = 5.0f;
	std::function<void()> function;
};

class TimerHandler 
{
public:
	/// <summary>
	/// Register a timer to perform a event at the end of the duration.
	/// </summary>
	/// <param name="_args"></param>
	void add_timer(DM::Utils::UUID _uuid, TimerArgs _args);

public:
	TimerHandler() = default;
	virtual ~TimerHandler() = default;

private:
	void update();

	std::unordered_map<DM::Utils::UUID, TimerArgs> m_Timers;

	friend class Client;
};