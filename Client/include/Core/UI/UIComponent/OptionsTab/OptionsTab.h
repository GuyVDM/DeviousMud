#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Shared/Utilities/EventListener.h"

struct OptionArgs
{
	std::string actionStr;
	SDL_Color   actionCol = { 255, 255, 0, 255 };

	std::string subjectStr;
	SDL_Color   subjectCol = { 255, 255, 255, 255 };

	std::function<void()> function;
};

/// <summary>
/// An entry to the option menu.
/// </summary>
class Option : public UIComponent
{
public:
	EventListener<void> on_clicked;

	virtual void on_hover() override;

	virtual void on_hover_end() override;

	virtual void on_left_click() override;

	virtual ~Option() = default;

	using UIComponent::UIComponent;

private:
	virtual void init();

	void set_option(OptionArgs _args);

private:
	OptionArgs m_optionArgs;

	friend class OptionsTab;
};

/// <summary>
/// Rightclick menu with all hovered interactable options displayed.
/// </summary>
class OptionsTab : public UIComponent
{
	/// <summary>
	/// Adds an entry to the options menu, if clicked it will send a packet to the server using the Action.
	/// </summary>
	/// <param name="_actionStr"></param>
	/// <param name="_subjectStr"></param>
	/// <param name="_action"></param>
	public: static void add_option(OptionArgs _args);
	
	/// <summary>
	/// Opens up the options menu.
	/// </summary>
	public: static void open_option_menu();
	
	/// <summary>
	/// Callback to whenever the option menu opens.
	/// </summary>
	private: static EventListener<void> on_options_show;

	/// <summary>
	/// A query of all options registered.
	/// </summary>
	private: static EventListener<OptionArgs> on_option_added;

public:
	void show();

	void close();

	virtual bool handle_event(const SDL_Event* _event) override;

	virtual ~OptionsTab() = default;

	using UIComponent::UIComponent;

protected:
	virtual void init() override;

	virtual void on_hover_end() override;

	void renderOutlines(std::shared_ptr<Graphics::Renderer> _renderer);

private:
	void set_options_delete_flag();

	void clampToViewport();

	/// <summary>
	/// Callback that's subscribed to the 'on_option_added' event.
	/// </summary>
	/// <param name="_args"></param>
	void create_option(OptionArgs _args);

	/// <summary>
	/// Rescale the option menu to match to the biggest child element.
	/// </summary>
	void regenerate_option_box();
	
	virtual const bool overlaps_rect(const int& _x, const int& _y) const override;
private:
	bool            m_flagToClose;
	Utilities::vec2 m_initialSize;
	Rect            m_boundingRectOptionsUI;
};