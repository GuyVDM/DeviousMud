#pragma once
#include "../vendor/SDL2/SDL.h"

class EventReceiver 
{
public:
	EventReceiver() = default;
	virtual ~EventReceiver() = default;
	virtual bool handle_event(const SDL_Event* _event);
};