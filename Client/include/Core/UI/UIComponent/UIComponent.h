#pragma once

#include "Core/Events/Clickable/Clickable.h"

#include "Shared/Utilities/EventListener.h"

enum class e_AnchorPreset : uint8_t
{
	TOP_LEFT      = 0x00,
	CENTER_LEFT   = 0x01,
	BOTTOM_LEFT   = 0x02,
	TOP_CENTER    = 0x03,
	CENTER        = 0x04,
	BOTTOM_CENTER = 0x05,
	TOP_RIGHT     = 0x06,
	CENTER_RIGHT  = 0x07,
	BOTTOM_RIGHT  = 0x08
};

struct Rect
{
	Utilities::vec2 minPos;
	Utilities::vec2 maxPos;

	// Copy assignment operator
	inline Rect& operator+(const Rect& _other)
	{
		minPos = minPos + _other.minPos;
		maxPos = maxPos + _other.maxPos;
		return *this;
	}

	const bool point_overlaps_rect(const Utilities::vec2& _point) const
	{
		return (_point.x >= minPos.x && _point.x <= maxPos.x && _point.y >= minPos.y && _point.y <= maxPos.y);
	}

	 const Utilities::vec2 get_size() const 
	 {
		 return Utilities::vec2(maxPos.x - minPos.x, maxPos.y - minPos.y);
	 }

};

namespace Graphics 
{
	class Renderer;

	namespace UI
	{
		class HUDLayer;
	}
}

class UIComponent : public Clickable
{
public:
	/// <summary>
	/// How this UI element should be rendered based on its position.
	/// </summary>
	enum e_RenderMode 
	{
		SCREENSPACE = 0,
		WORLDSPACE
	};

	using Position = Utilities::vec2;
	using Size     = Utilities::vec2;

	template <typename T>
	static std::shared_ptr<T> create_component(const Utilities::vec2 _pos, const Utilities::vec2 _size, Graphics::SpriteType _sprite, bool _bInteractable = false);

	/// <summary>
	/// Allows for indentification of asset whilest debugging.
	/// </summary>
	/// <param name="_debugName"></param>
	void set_asset_name(std::string _debugName);

	/// <summary>
	/// Generates a anchor based on the preset and the parents transform.
	/// </summary>
	/// <param name="_preset"></param>
	/// <returns></returns>
	static Utilities::vec2 get_anchor_position(e_AnchorPreset _preset, Rect _rect);

	/// <summary>
	/// Rendering priority of the sprite on the z axis.
	/// </summary>
	/// <param name="_zOrder"></param>
	void set_z_order(int32_t _zOrder);

	/// <summary>
	/// Returns the top object in the hierarchy.
	/// </summary>
	/// <returns></returns>
	const UIComponent* get_root() const;

	/// <summary>
	/// Registers the received pointer as a child.
	/// </summary>
	/// <param name="_component"></param>
	void add_child(std::shared_ptr<UIComponent> _component);

	/// <summary>
	/// Attempts to remove the component from the children if it's valid.
	/// </summary>
	/// <param name="_component"></param>
	void remove_child(UIComponent* _component);

	/// <summary>
	/// Will try and return the first derived UIComponent of a certain type. 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<typename T>
	std::optional<std::shared_ptr<T>> get_component_in_children();

	/// <summary>
	/// Set the anchor of the UI element.
	/// </summary>
	/// <param name=""></param>
	void set_anchor(e_AnchorPreset _anchorPreset);

	/// <summary>
	/// Check if a event is handled by this listener.
	/// </summary>
	/// <param name="_event"></param>
	/// <returns></returns>
	virtual bool handle_event(const SDL_Event* _event) override;

	/// <summary>
	/// Return the number of children.
	/// </summary>
	/// <returns></returns>
	const int32_t get_child_count() const;

	/// <summary>
	/// Changes the position of the UI element.
	/// Updates the parent & the children accordingly.
	/// </summary>
	/// <param name="_pos"></param>
	virtual void set_position(Utilities::vec2 _pos) final override;

	/// <summary>
	/// Changes the size of this UI element.
	/// Updates the parent & the children accordingly.
	/// </summary>
	/// <param name="_size"></param>
	virtual void set_size(Utilities::vec2 _size) final override;

	/// <summary>
	/// Returns the rectangular surface area that's occupied by this element specifically
	/// </summary>
	/// <returns></returns>
	const Rect get_local_rect() const;

	/// <summary>
	/// Returns the complete rectangular surface area that's occupied by the ui element and its children.
	/// </summary>
	/// <returns></returns>
	const Rect get_bounding_rect() const;

	/// <summary>
	/// Whether is UIElement should be executed and displayed.
	/// </summary>
	/// <param name="_bIsActive"></param>
	void set_active(bool _bIsActive);

	/// <summary>
	/// Wrapper function for rendering.
	/// </summary>
	void render();

	/// <summary>
	/// Unchilds and destroys the object.
	/// </summary>
	void unparent();

	/// <summary>
	/// Whether you want to render the UI screen or worldspace.
	/// </summary>
	/// <param name="_eRenderMode"></param>
	void set_render_mode(e_RenderMode _eRenderMode);

	/// <summary>
	/// Makes the transform of this object relative to the parent.
	/// </summary>
	/// <param name="_parent"></param>
	void set_parent(UIComponent* _parent);

	/// <summary>
	/// Main loop call.
	/// </summary>
	/// <returns></returns>
	void recursiveUpdateCleanup();


	virtual ~UIComponent();

	UIComponent(const Utilities::vec2& _pos, const Utilities::vec2& _size, Graphics::SpriteType _sprite);

protected:
	/// <summary>
	/// Eventlistener to bind any additional rendering related calls to.
	/// </summary>
	EventListener<std::shared_ptr<Graphics::Renderer>> on_render_call;

	/// <summary>
	/// Initialise the component.
	/// </summary>
	virtual void init();
	
private:
	/// <summary>
	/// Happens every frame.
	/// </summary>
	virtual void update();
	
	/// <summary>
	/// At the end of each frame, deparents any removed children.
	/// </summary>
	void on_cleanup();

	/// <summary>
	/// Renders the UI element.
	/// </summary>
	void render(std::shared_ptr<Graphics::Renderer> _renderer);

	/// <summary>
	/// Reference to UI component that's getting moved.
	/// </summary>
	static UIComponent* s_draggedComponent;

	/// <summary>
	/// The point of offset 
	/// </summary>
	static Utilities::vec2 s_dragOffset;

	/// <summary>
	/// Make children adjust to any potential changes. Call when a change related to the position or scale has been made.
	/// </summary>
	void update_children(Rect _oldRect);

	/// <summary>
	/// Overlap rect is transformed to detect screenspace only.
	/// </summary>
	/// <param name="_x"></param>
	/// <param name="_y"></param>
	/// <returns></returns>
	virtual const bool overlaps_rect(const int& _x, const int& _y) const override;

	template<typename T>
	friend std::shared_ptr<T> UIComponent::create_component(const Utilities::vec2 _pos, const Utilities::vec2 _size, Graphics::SpriteType _sprite, bool _bInteractable);

	friend UIComponent;
	friend std::shared_ptr<UIComponent>;
	friend Graphics::UI::HUDLayer;

protected:
	std::string                                m_assetName = "Null";

	bool                                       m_bIsActive     = true;
	bool                                       m_bIsMovable    = false;
	bool									   m_bInteractable = true;
	UIComponent*                               m_parent;
	std::vector<std::shared_ptr<UIComponent>>  m_children;
	e_RenderMode                               m_eRenderMode = e_RenderMode::SCREENSPACE;
	
private:
	std::vector<UIComponent*> toRemove;

	e_AnchorPreset                             m_anchor      = e_AnchorPreset::TOP_LEFT;
};

template<typename T>
inline std::shared_ptr<T> UIComponent::create_component(const Utilities::vec2 _pos, const Utilities::vec2 _size, Graphics::SpriteType _sprite, bool _bInteractable)
{
	static_assert(std::is_base_of<UIComponent, T>::value, "T must inherit from the UIComponent.");

	T* component = new T(_pos, _size, _sprite);

	// Cast back to base so we can call the init function since the base is a friend.

	UIComponent* base = dynamic_cast<UIComponent*>(component);
	base->m_bInteractable = _bInteractable;
	base->init();

	return std::shared_ptr<T>(component);
}

template<typename T>
inline std::optional<std::shared_ptr<T>> UIComponent::get_component_in_children() 
{
	for(std::shared_ptr<UIComponent> component : m_children) 
	{
		if(std::shared_ptr<T> derivedComponent = std::dynamic_pointer_cast<T>(component)) 
		{
			return derivedComponent;
		}
	}

	return std::nullopt;
}