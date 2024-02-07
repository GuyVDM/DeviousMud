#pragma once
#include "Core/Events/EventReceiver.h"
#include "Core/Rendering/Sprite/Sprite.h"

#include "Shared/Utilities/vec2.hpp"

class Clickable : public EventReceiver 
{
public:
	bool handle_event(const SDL_Event* _event) override;

	const Graphics::Sprite& get_sprite() const;

	const Utilities::vec2& get_pos() const;

	const Utilities::vec2& get_size() const;

public:
	explicit Clickable(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);
	virtual ~Clickable();

protected:
	virtual void on_hover();

	virtual void on_stop_hover();

	virtual void on_left_click();

	virtual void on_right_click();

protected:
	Utilities::vec2   size;
	Utilities::vec2	  pos;
	Graphics::Sprite  sprite;

private:
	const bool overlaps_rect(const int& _x, const int& _y) const;

private:
	bool is_hovered;
};