#pragma once
#include "precomp.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

using namespace Graphics;

EventListener<OptionArgs> OptionsTab::on_option_added;

void OptionsTab::add_option(std::string _actionStr, std::string _subjectStr, const DM::Actions::Action& _action)
{
	OptionArgs args;
	args.action = _action;
	args.actionStr = _actionStr;
	args.subjectStr = _subjectStr;

	on_option_added.invoke(args);
}

void OptionsTab::init()
{
	//Bind callback
	on_option_added.add_listener
	(
		std::bind(&OptionsTab::create_option, this, std::placeholders::_1)
	);

	//Create visuals
	{
		std::shared_ptr<UIComponent> component;
		//Create options header 
		{
			TextArgs args;
			args.color = { 131, 133, 134 , 255 };
			args.font = e_FontType::RUNESCAPE_UF;
			args.size = 25;
			component = TextComponent::create_text("Choose Option:", get_position() + Utilities::vec2(10.0f, 2.5f), args);
		}	add_child(component);

		set_size(component->get_size() + Utilities::vec2(20.0f, 5.0f));

		bIsMovable = true;

		DM::Actions::Action action;
		action.action = DM::Actions::e_ActionType::ATTACK;
		action.playerId = DM::Utils::UUID::generate();
		action.subjectId = DM::Utils::UUID::generate();
		action.subject = DM::Actions::e_SubjectType::PLAYER;

		OptionArgs args;
		args.action = action;
		args.actionStr = "Attack";
		args.subjectStr = "Test";

		create_option(args);

		create_option(args);
	}
}

void OptionsTab::create_option(OptionArgs _args)
{
	Utilities::vec2 size = get_size();

	std::shared_ptr<Option> option = UIComponent::create_component<Option>	
	(
		get_position(),
		get_size(),
		SpriteType::HUD_OPTIONS_BOX
	);
	add_child(option);

	option->set_option(_args);
}

void Option::on_hover()
{
	sprite.color = { 255, 255, 255, 255 };
}

void Option::on_hover_end()
{
	sprite.color = { 200, 200, 200, 255 };
}

void Option::init()
{
	sprite.color = { 200, 200, 200, 255 };
}

void Option::set_option(OptionArgs _args)
{
	args = _args;

	//Create visuals
	{
		TextArgs args;
		args.color = { 131, 133, 134 , 255 };
		args.font = e_FontType::RUNESCAPE_UF;
		args.size = 15;

		std::shared_ptr<UIComponent> component;
		//Create options header 
		{
			Utilities::vec2 position = parent->get_position() + Utilities::vec2(0.0f, parent->get_size().y);
			position += Utilities::vec2(0.0f, get_size().y) * parent->get_child_count() - 1;
			set_position(position);

			//Create text
			component = TextComponent::create_text(_args.actionStr.c_str(), get_position() + Utilities::vec2(10.0f, 2.5f), args);
			set_size(component->get_size() + Utilities::vec2(20.0f, (float)args.size / 2.0f));

		}	add_child(component);
	}
}
