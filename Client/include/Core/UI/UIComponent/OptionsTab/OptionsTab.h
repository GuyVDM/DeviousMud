#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/Actions.h"

struct OptionArgs
{
	std::string actionStr;
	std::string subjectStr;
	DM::Actions::Action action;
};

class Option : public UIComponent
{
public:
	virtual void on_hover() override;

	virtual void on_hover_end() override;

	virtual ~Option() = default;

	using UIComponent::UIComponent;

private:
	virtual void init();

	void set_option(OptionArgs _args);

private:

	OptionArgs args;

	friend class OptionsTab;
};

/// <summary>
/// Rightclick menu with all hovered interactable options displayed.
/// </summary>
class OptionsTab : public UIComponent
{
public:
	/// <summary>
	/// Adds an entry to the options menu, if clicked it will send a packet to the server using the Action.
	/// </summary>
	/// <param name="_actionStr"></param>
	/// <param name="_subjectStr"></param>
	/// <param name="_action"></param>
	static void add_option(std::string _actionStr, std::string _subjectStr, const DM::Actions::Action& _action);

	virtual ~OptionsTab() = default;

	using UIComponent::UIComponent;

private:
	static EventListener<OptionArgs> on_option_added;

	virtual void init();

	void create_option(OptionArgs _args);

};