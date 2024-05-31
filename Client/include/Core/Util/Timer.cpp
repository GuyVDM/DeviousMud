#include "precomp.h"

#include "Core/Util/TimerHandler.h"

void TimerHandler::add_timer(DM::Utils::UUID _uuid, TimerArgs _args)
{
	m_Timers[_uuid]  =_args;
}

void TimerHandler::update()
{
	std::vector<DM::Utils::UUID> toRemove;

	for(auto&[uuid, timer] : m_Timers) 
	{
		timer.duration -= DM::CLIENT::Config::get_deltaTime();

		if(timer.duration <= 0) 
		{
			timer.function();
			toRemove.push_back(uuid);
		}
	}

	for(DM::Utils::UUID& uuid : toRemove) 
	{
		m_Timers.erase(uuid);
	}
}
