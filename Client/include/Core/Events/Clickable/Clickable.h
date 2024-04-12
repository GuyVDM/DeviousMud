#pragma once
#include "Core/Events/EventReceiver.h"

#include "Core/Rendering/Sprite/Sprite.h"

#include "Shared/Utilities/vec2.hpp"

class Clickable : public EventReceiver 
{
public:
	virtual bool handle_event(const SDL_Event* _event) override;

	const Graphics::Sprite& get_sprite() const;

	const Utilities::vec2&  get_position() const;

	const Utilities::vec2&  get_size() const;

	virtual void set_position(Utilities::vec2 _pos);

	virtual void set_size(Utilities::vec2 _size);

public:
	explicit Clickable(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);
	virtual ~Clickable();

protected:
	virtual void on_hover();

	virtual void on_hover_end();

	virtual void on_left_click();

	virtual void on_right_click();

	const bool is_hovered() const;

protected:
	Graphics::Sprite  sprite;

private:
	Utilities::vec2   size;
	Utilities::vec2	  pos;
	bool bIsHovered;

private:
	virtual const bool overlaps_rect(const int& _x, const int& _y) const;

};