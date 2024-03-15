#include "precomp.h"

#include "Core/UI/UIComponent/UIComponent.h"

const bool UIComponent::overlaps_rect(const int& _x, const int& _y) const
{
    // Calculate sprite's screen coordinates
    int32_t halfExtendsWidth = static_cast<int32_t>(scale.x / 2.0f);
    int32_t halfExtendsHeight = static_cast<int32_t>(scale.y / 2.0f);

    Utilities::ivec2 transformedPos;
    transformedPos.x = static_cast<int32_t>(pos.x);
    transformedPos.y = static_cast<int32_t>(pos.y);
    transformedPos.x += halfExtendsWidth;
    transformedPos.y += halfExtendsHeight;

    // Calculate sprite's bounding box
    const int32_t left = transformedPos.x - halfExtendsWidth;
    const int32_t right = transformedPos.x + halfExtendsWidth;
    const int32_t top = transformedPos.y - halfExtendsHeight;
    const int32_t bottom = transformedPos.y + halfExtendsHeight;

    return (_x > left && _x < right && _y > top && _y < bottom);
}

UIComponent::UIComponent(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::Sprite _sprite) : Clickable(_pos, _size, _sprite)
{
}
