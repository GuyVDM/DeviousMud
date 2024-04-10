#include "precomp.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

UIComponent*    UIComponent::sDraggedComponent = nullptr;
Utilities::vec2 UIComponent::sDragOffset       = Utilities::vec2(0.0f);

const int32_t UIComponent::get_child_count() const
{
    return children.size();
}

UIComponent::UIComponent(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::SpriteType _sprite) :
    Clickable(_pos, _size, g_globals.renderer.lock()->get_sprite(_sprite))
{
    set_position(_pos);
}

void UIComponent::init()
{
}

const bool UIComponent::overlaps_rect(const int& _x, const int& _y) const
{
    const Utilities::vec2 size = get_size();
    const Utilities::vec2 pos  = get_position();

    // Calculate sprite's screen coordinates
    int32_t halfExtendsWidth  = static_cast<int32_t>(size.x / 2.0f);
    int32_t halfExtendsHeight = static_cast<int32_t>(size.y / 2.0f);

    Utilities::ivec2 transformedPos;
    transformedPos.x = static_cast<int32_t>(pos.x);
    transformedPos.y = static_cast<int32_t>(pos.y);
    transformedPos.x += halfExtendsWidth;
    transformedPos.y += halfExtendsHeight;

    transformedPos = transformedPos;

    // Calculate sprite's bounding box
    const int32_t left = transformedPos.x - halfExtendsWidth;
    const int32_t right = transformedPos.x + halfExtendsWidth;
    const int32_t top = transformedPos.y - halfExtendsHeight;
    const int32_t bottom = transformedPos.y + halfExtendsHeight;

    return (_x > left && _x < right && _y > top && _y < bottom);
}

Utilities::vec2 UIComponent::get_anchor_position(e_AnchorPreset _preset, Rect _rect)
{
    switch (_preset)
    {
    case e_AnchorPreset::TOP_LEFT:
        return Utilities::vec2(_rect.minPos.x, _rect.minPos.y);

    case e_AnchorPreset::CENTER_LEFT:
        return Utilities::vec2(_rect.minPos.x, (_rect.minPos.y + _rect.maxPos.y) / 2.0f);

    case e_AnchorPreset::BOTTOM_LEFT:
        return Utilities::vec2(_rect.minPos.x, _rect.maxPos.y);

    case e_AnchorPreset::TOP_CENTER:
        return Utilities::vec2((_rect.minPos.x + _rect.maxPos.x) / 2.0f, _rect.minPos.y);

    case e_AnchorPreset::CENTER:
        return Utilities::vec2((_rect.minPos.x + _rect.maxPos.x) / 2.0f, (_rect.minPos.y + _rect.maxPos.y) / 2.0f);

    case e_AnchorPreset::BOTTOM_CENTER:
        return Utilities::vec2((_rect.minPos.x + _rect.maxPos.x) / 2.0f, _rect.maxPos.y);

    case e_AnchorPreset::TOP_RIGHT:
        return Utilities::vec2(_rect.maxPos.x, _rect.minPos.y);

    case e_AnchorPreset::CENTER_RIGHT:
        return Utilities::vec2(_rect.maxPos.x, (_rect.minPos.y + _rect.minPos.y) / 2.0f);

    case e_AnchorPreset::BOTTOM_RIGHT:
        return Utilities::vec2(_rect.maxPos.x, _rect.maxPos.y);
    }

    DEVIOUS_ERR("Anchor preset not implemented.");
    return Utilities::vec2(0.0f);
}


const UIComponent* UIComponent::get_root() const
{
    if (parent)
    {
        return parent->get_root();
    }
    else
    {
        return this;
    }
}

void UIComponent::set_parent(UIComponent* _parent)
{
    parent = _parent;
}

void UIComponent::set_anchor(e_AnchorPreset _anchorPreset)
{
    anchor = _anchorPreset;
}

void UIComponent::add_child(std::shared_ptr<UIComponent> _component)
{
    auto it = std::find_if(children.begin(), children.end(), [_component](const std::shared_ptr<UIComponent> _other)
        {
            return _component == _other;
        });

    if(it == children.end()) 
    {   
        children.push_back(_component);
        _component->set_parent(this);
        return;
    }

    DEVIOUS_WARN("The given UIComponent is already a child of this object!");
}

void UIComponent::remove_child(std::shared_ptr<UIComponent> _component)
{
    auto it = std::find_if(children.begin(), children.end(), [_component](const std::shared_ptr<UIComponent> _other)
        {
            return _component == _other;
        });

    if (it != children.end())
    {
        children.erase(it);
    }
}

const Rect UIComponent::get_bounding_rect() const
{
    const Utilities::vec2 pos = get_position();
    const Utilities::vec2 size = get_size();

    Rect rect
    {
        pos,
        Utilities::vec2(pos.x + size.x, pos.y + size.y)
    };

    for (std::shared_ptr<UIComponent> child : children)
    {
        const Rect childRect = child->get_bounding_rect();

        rect.minPos.x = std::min(rect.minPos.x, childRect.minPos.x);
        rect.minPos.y = std::min(rect.minPos.y, childRect.minPos.y);
        rect.maxPos.x = std::max(rect.maxPos.x, childRect.maxPos.x);
        rect.maxPos.y = std::max(rect.maxPos.y, childRect.maxPos.y);
    }

    return rect;
}

const Rect UIComponent::get_local_rect() const
{
    const Utilities::vec2 pos  = get_position();
    const Utilities::vec2 size = get_size();

    return Rect
    {
        pos,
        Utilities::vec2(pos.x + size.x, pos.y + size.y)
    };;
}

void UIComponent::render(std::shared_ptr<Graphics::Renderer> _renderer)
{
    const bool renderOutline =  (Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::DISPLAY ||
                                 Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::MOVE) && bIsMovable;

    if (renderOutline)
    {
        const int outlineSizePx = 3;
        const SDL_Color yellow = { 255, 255, 0, 255 };
        const Rect boundingRect = get_bounding_rect();
        const Utilities::vec2 pos = boundingRect.minPos;
        const Utilities::vec2 size = boundingRect.maxPos - boundingRect.minPos;

        _renderer->draw_outline(pos, size, outlineSizePx, yellow);
    }

    // Render UIComponent Sprite
    _renderer->plot_raw_frame(get_sprite(), get_position(), get_size());

    for (auto child : children)
    {
        child->render(_renderer);
    }
}

bool UIComponent::handle_event(const SDL_Event* _event)
{
    if (Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::MOVE)
    {
        if (bIsMovable)
        {
            if (!UIComponent::sDraggedComponent)
            {
                Utilities::ivec2 mousePos;
                SDL_GetMouseState(&mousePos.x, &mousePos.y);

                //If the mouse overlaps anywhere of the entirety of this UI element.
                if (get_bounding_rect().point_overlaps_rect(Utilities::to_vec2(mousePos)))
                {
                    UIComponent::sDragOffset = Utilities::to_vec2(mousePos) - get_position();
                    UIComponent::sDraggedComponent = this;
                    return true;

                }
            }
        }
    }
    else 
    if (UIComponent::sDraggedComponent == this)
    {
        UIComponent::sDragOffset = Utilities::vec2(0.0f);
        UIComponent::sDraggedComponent = nullptr;
    }

    //Check if any of the children handle it first.
    for (const auto& child : children)
    {
        if(child->handle_event(_event))
        {
            return true;
        }
    }

    //If children didn't handle the event, check if we can handle it ourselves.
    //IF this an interactable ui.
    if (bInteractable) 
    {
        return Clickable::handle_event(_event);
    }

    return false;
}

void UIComponent::update_children(Rect _oldRect)
{
    for (auto& child : children)
    {
        const e_AnchorPreset childAnchor = child->anchor;
        const Rect oldChildRect = child->get_local_rect();
        const Utilities::vec2 oldAnchorPoint = get_anchor_position(childAnchor, _oldRect);
        const Utilities::vec2 newAnchorPoint = get_anchor_position(childAnchor, get_local_rect());

        Utilities::vec2 oldOffset = child->get_position() - oldAnchorPoint;
        Utilities::vec2 newPos = newAnchorPoint + oldOffset;
        child->Clickable::set_position(newPos);

        child->update_children(oldChildRect);
    }
}

void UIComponent::set_size(Utilities::vec2 _size)
{
    Rect oldRect = get_local_rect();

    Clickable::set_size(_size);

    update_children(oldRect);
}

void UIComponent::set_position(Utilities::vec2 _pos)
{
    Rect oldRect = get_local_rect();

    Clickable::set_position(_pos);

    update_children(oldRect);
}


