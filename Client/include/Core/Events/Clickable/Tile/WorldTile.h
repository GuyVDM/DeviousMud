#pragma once
#include "Core/Events/Clickable/Clickable.h"

#include <memory>

class WorldTile final : public Clickable 
{
public:
	static std::shared_ptr<WorldTile> create_tile(const Utilities::vec2& _position);

public:
	virtual void on_hover();

	virtual void on_hover_end();

	virtual void on_left_click();

	virtual bool handle_event(const SDL_Event* _event) override;

public:
	using Clickable::Clickable;
	~WorldTile();
};