#pragma once
#include "Core/Events/Clickable/Clickable.h"

class UIComponent : public Clickable 
{
public:
	/// <summary>
	/// Overlap rect is transformed to detect screenspace only.
	/// </summary>
	/// <param name="_x"></param>
	/// <param name="_y"></param>
	/// <returns></returns>
	virtual const bool overlaps_rect(const int& _x, const int& _y) const override;

public:
	UIComponent(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite);
	virtual ~UIComponent() = default;

};