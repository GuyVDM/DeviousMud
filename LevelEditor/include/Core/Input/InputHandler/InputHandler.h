#pragma once
#include "Core/Core.h"

typedef union SDL_Event SDL_Event;

class InputHandler
{
public:
	virtual void ExtractInput(const SDL_Event& _event);

	virtual void Update();

public:
	InputHandler() = default;
	virtual ~InputHandler() = default;
};