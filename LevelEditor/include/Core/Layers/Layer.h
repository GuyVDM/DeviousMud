#pragma once
typedef union SDL_Event SDL_Event;

#define IMPLEMENT_LAYER_FUNCTIONS \
const virtual bool HandleEvent(const SDL_Event& _event) override; \
virtual void Update() override; \

class Layer
{
public:
	const virtual bool HandleEvent(const SDL_Event& _event) = 0;

	virtual void Update() = 0;

public:
	Layer() = default;
	virtual ~Layer() = default;
};