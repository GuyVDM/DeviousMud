#pragma once
#include "Core/Events/Clickable/Clickable.h"

class WorldEntity : public Clickable 
{
public:
	virtual void on_hover();

	virtual void on_hover_end();

	virtual void on_left_click();

	virtual void on_right_click();

	virtual void set_position();

public:
	using Clickable::Clickable;
	~WorldEntity();

};