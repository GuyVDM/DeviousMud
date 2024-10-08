#include "precomp.h"

#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/UI/Layer/HUDLayer/HUDLayer.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

UIComponent*    UIComponent::s_draggedComponent = nullptr;
Utilities::vec2 UIComponent::s_dragOffset       = Utilities::vec2(0.0f);

void UIComponent::init()
{
}

void UIComponent::on_new_parent()
{
}

void UIComponent::update()
{
}

const int32_t UIComponent::get_child_count() const
{
    return static_cast<int32_t>(m_children.size());
}


void UIComponent::set_active(bool _bIsActive)
{
    m_bIsActive = _bIsActive;
}

void UIComponent::render()
{
    render(g_globals.renderer.lock());
}

void UIComponent::recursiveUpdateCleanup()
{
    on_cleanup();

    update();

    for(const auto& child : m_children) 
    {
        child->on_cleanup();

        child->update();
    }
}

UIComponent::~UIComponent()
{
    on_render_call.clear();
}

UIComponent::UIComponent(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::SpriteType _sprite) :
    Clickable(_pos, _size, g_globals.renderer.lock()->get_sprite(_sprite))
{
    set_position(_pos);
}

void UIComponent::on_cleanup()
{
    const int32_t removeSize = static_cast<int32_t>(toRemove.size()) - 1;

    for (int32_t i = removeSize; i >= 0; i--)
    {
        if(toRemove[i]) 
        {
            remove_child(toRemove[i]);
            toRemove.erase(toRemove.begin() + i);
        }
    }
}

const bool UIComponent::overlaps_rect(const int& _x, const int& _y) const
{
    const Utilities::ivec2 halfExtends = Utilities::to_ivec2(Utilities::vec2(get_size() / 2.0f));
    const Utilities::ivec2 pos         = Utilities::to_ivec2(get_position());

    const Utilities::ivec2 transformedPos
    {
        pos.x + halfExtends.x,
        pos.y + halfExtends.y
    };

    // Calculate sprite's bounding box
    const int32_t left   = transformedPos.x - halfExtends.x;
    const int32_t right  = transformedPos.x + halfExtends.x;
    const int32_t top    = transformedPos.y - halfExtends.y;
    const int32_t bottom = transformedPos.y + halfExtends.y;

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

void UIComponent::set_z_order(int32_t _zOrder)
{
    m_sprite.zRenderPriority = _zOrder;
}

const UIComponent* UIComponent::get_root() const
{
    if (m_parent)
    {
        return m_parent->get_root();
    }
    else
    {
        return this;
    }
}

void UIComponent::set_parent(UIComponent* _parent)
{
    m_parent      = _parent;

    // Copy some parent essentials.
    {
        m_eRenderMode            = m_parent->m_eRenderMode;
        m_sprite.zRenderPriority = _parent->m_sprite.zRenderPriority;

        for (auto& child : m_children)
        {
            child->m_eRenderMode = m_eRenderMode;
            child->m_sprite.zRenderPriority = m_sprite.zRenderPriority;
        }
    }

    on_new_parent();
}

void UIComponent::unparent()
{
    if(m_parent) 
    {
        m_parent->toRemove.push_back(this);
    }
}

void UIComponent::set_render_mode(e_RenderMode _eRenderMode)
{
    m_eRenderMode = _eRenderMode;
}

void UIComponent::set_anchor(e_AnchorPreset _anchorPreset)
{
    m_anchor = _anchorPreset;
}

void UIComponent::add_child(std::shared_ptr<UIComponent> _component, int32_t _insertAt)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), [_component](const std::shared_ptr<UIComponent> _other)
        {
            return _component == _other;
        });

    if(it == m_children.end()) 
    {   
        if(_insertAt > -1) 
        {
            m_children.insert(m_children.begin() + _insertAt, _component);
        }
        else
        {
            m_children.push_back(_component);
        }

        _component->set_parent(this);
        return;
    }

    DEVIOUS_WARN("The specified UIComponent is already a child of this object.");
}

void UIComponent::remove_child(UIComponent* _component)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), [_component](const std::shared_ptr<UIComponent> _other)
        {
            return _component == _other.get();
        });

    if (it != m_children.end())
    {
        const int64_t index = std::distance(m_children.begin(), it);
        m_children.erase(std::remove(m_children.begin(), m_children.end(), m_children[index]), m_children.end());
        return;
    }

    DEVIOUS_ERR("Failed to remove component.");
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

    for (std::shared_ptr<UIComponent> child : m_children)
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
    const Utilities::vec2 pos = get_position();
    const Utilities::vec2 size = get_size();

    return Rect
    {
        pos,
        Utilities::vec2(pos.x + size.x, pos.y + size.y)
    };;
}

void UIComponent::render(std::shared_ptr<Graphics::Renderer> _renderer)
{
    if (m_bIsActive) 
    {
        const bool renderOutline = (Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::DISPLAY ||
                                    Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::MOVE) &&
                                    m_bIsMovable;

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
        switch(m_eRenderMode) 
        {
        case e_RenderMode::SCREENSPACE:
            _renderer->plot_raw_frame(get_sprite(), get_position(), get_size());
            break;

        case e_RenderMode::WORLDSPACE:
            _renderer->plot_frame(get_sprite(), get_position(), get_size());
            break;
        }

        for (const auto& child : m_children)
        {
            child->render(_renderer);
        }

        /// If you want to do additional rendering.
        /// Bind any external rendering function to this within the derived classes.
        /// ------------------------------------------------------------------------
        on_render_call.invoke(_renderer);
    }
}

bool UIComponent::handle_event(const SDL_Event* _event)
{
    if (m_bIsActive)
    {
        if (Graphics::UI::HUDLayer::get_interaction_type() == e_UIInteractionType::MOVE)
        {
            if (m_bIsMovable)
            {
                if (!UIComponent::s_draggedComponent)
                {
                    Utilities::ivec2 mousePos;
                    SDL_GetMouseState(&mousePos.x, &mousePos.y);

                    //If the mouse overlaps anywhere of the entirety of this UI element.
                    if (get_bounding_rect().point_overlaps_rect(Utilities::to_vec2(mousePos)))
                    {
                        UIComponent::s_dragOffset = Utilities::to_vec2(mousePos) - get_position();
                        UIComponent::s_draggedComponent = this;
                        return true;
                    }
                }
            }
        }
        else
            if (UIComponent::s_draggedComponent == this)
            {
                UIComponent::s_dragOffset = Utilities::vec2(0.0f);
                UIComponent::s_draggedComponent = nullptr;
            }

        //Check if any of the children handle it first.
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
        {
            const auto& child = *it;

            if (child->handle_event(_event))
            {
                return true;
            }
        }

        //If children didn't handle the event, check if we can handle it ourselves.
        //IF this an interactable ui.
        if (m_bInteractable)
        {
            return Clickable::handle_event(_event);
        }
    }

    return false;
}

void UIComponent::update_children(Rect _oldRect)
{
    for (auto& child : m_children)
    {
        const e_AnchorPreset childAnchor = child->m_anchor;
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


