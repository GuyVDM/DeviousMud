#pragma once
#include "Core/Events/Clickable/Clickable.h"

#include <memory>

class WorldTile final : public Clickable 
{
public:
	static std::shared_ptr<WorldTile> create_tile(const Utilities::vec2& position);

public:
	virtual void on_hover();

	virtual void on_hover_end();

	virtual void on_left_click();


public:
	using Clickable::Clickable;
	~WorldTile();
};