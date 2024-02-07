#pragma once
#include "Core/Events/Clickable/Clickable.h"

#include <memory>

class WorldTile final : public Clickable 
{
public:
	static std::shared_ptr<WorldTile> create_tile(const Utilities::vec2& position);

public:
	virtual void on_hover();

	virtual void on_stop_hover();

	virtual void on_left_click();

	/// <summary>
	/// Returns the tile position gridspace. 
	/// </summary>
	/// <returns></returns>
	const Utilities::ivec2& get_tile_position() const;

	/// <summary>
	/// Returns the precise coordinates of the central point of the tile.
	/// </summary>
	/// <returns></returns>
	const Utilities::vec2& get_center_position() const;


public:
	using Clickable::Clickable;
	~WorldTile();
};