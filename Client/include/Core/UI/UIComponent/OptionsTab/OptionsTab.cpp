#pragma once
#include "precomp.h"

#include "Core/UI/UIComponent/OptionsTab/OptionsTab.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Rendering/Renderer.h"

using namespace Graphics;

EventListener<OptionArgs> OptionsTab::on_option_added;
EventListener<void>       OptionsTab::on_options_show;

void OptionsTab::add_option(std::string _actionStr, std::string _subjectStr, const DM::Actions::Action& _action)
{
	OptionArgs args;
	args.actionType = _action;
	args.actionStr = _actionStr;
	args.subjectStr = _subjectStr;

	on_option_added.invoke(args);
}

void OptionsTab::open_option_menu()
{
	on_options_show.invoke();
}

void OptionsTab::init()
{
	//Set up callbacks
	{
		on_render_call.add_listener
		(
			std::bind(&OptionsTab::renderOutlines, this, std::placeholders::_1)
		);

		on_option_added.add_listener
		(
			std::bind(&OptionsTab::create_option, this, std::placeholders::_1)
		);

		on_options_show.add_listener
		(
			std::bind(&OptionsTab::show, this)
		);
	}

	//Set up visuals
	{
		sprite.color = { 0, 0, 0, 0 };
		std::shared_ptr<UIComponent> component;
		//Create options header 
		{
			TextArgs args;
			{
				args.color = { 93, 84, 71 , 255 };
				args.font = e_FontType::RUNESCAPE_UF;
				args.size = 25;
			}

			component = TextComponent::create_text("Choose Option", get_position() + Utilities::vec2(10.0f, 2.5f), args);
			add_child(component);
			set_size(component->get_size() + Utilities::vec2(20.0f, 5.0f));
			component->set_anchor(e_AnchorPreset::CENTER_LEFT);
		}
	}

	close();
}

void OptionsTab::on_hover_end()
{
	if(bIsActive) 
	{
		close();
	}
}

void OptionsTab::create_option(OptionArgs _args)
{
	const Utilities::vec2 size = get_size();

	std::shared_ptr<Option> option = UIComponent::create_component<Option>	
	(
		get_position(),
		get_size(),
		SpriteType::HUD_OPTIONS_BOX,
		true
	);  add_child(option);

	option->on_clicked.add_listener
	(
		std::bind(&OptionsTab::close, this)
	);

	//Register the option
	option->set_option(_args);

	//Move the option to the latest dropdown slot.
	const Utilities::vec2 position = get_position() + Utilities::vec2(0.0f, 2.0f) + (Utilities::vec2(0.0f, size.y) * (get_child_count() - 1));
	option->set_position(position);

	regenerate_option_box();
}

void OptionsTab::regenerate_option_box()
{
	Rect biggestRect { Utilities::vec2(FLT_MAX), get_position() };
	const float pxEdgeOffset = 5.0f;

	//We start at 1 since the header text element takes slot 0 always.
	for (int i = 1; i < children.size(); i++) 
	{
		if (children[i]) 
		{
			biggestRect.minPos.x = std::min(biggestRect.minPos.x, children[i]->get_bounding_rect().minPos.x);
			biggestRect.minPos.y = std::min(biggestRect.minPos.y, children[i]->get_bounding_rect().minPos.y);
			biggestRect.maxPos.x = std::max(biggestRect.maxPos.x, children[i]->get_bounding_rect().maxPos.x);
			biggestRect.maxPos.y = std::max(biggestRect.maxPos.y, children[i]->get_bounding_rect().maxPos.y);
		}
	}	

	biggestRect.maxPos.x += pxEdgeOffset;
	boundingRectOptions = biggestRect;

	//Scale all UI elements horizontally to match the biggest item.
	for (int i = 1; i < children.size(); i++)
	{
		const Utilities::vec2 childSizeOld = children[i]->get_size();
		children[i]->set_size(Utilities::vec2(biggestRect.get_size().x, childSizeOld.y));
	}   
	
	set_size(Utilities::vec2(biggestRect.get_size().x, get_size().y));
}

const bool OptionsTab::overlaps_rect(const int& _x, const int& _y) const
{
	const Rect boundingRect = get_bounding_rect();

	const Utilities::vec2 pos = boundingRect.minPos;
	const Utilities::vec2 size = boundingRect.get_size();

	// Calculate sprite's screen coordinates
	int32_t halfExtendsWidth = static_cast<int32_t>(size.x / 2.0f);
	int32_t halfExtendsHeight = static_cast<int32_t>(size.y / 2.0f);

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

void OptionsTab::show()
{
	close();

	if (!bIsActive) 
	{
		const Utilities::vec2 offset = Utilities::vec2(get_bounding_rect().get_size().x / 2.0f, 0.0f);

		Utilities::ivec2 mousePos;
		SDL_GetMouseState(&mousePos.x, &mousePos.y);

		bIsActive = true;
		set_position(Utilities::to_vec2(mousePos) - offset);
	}
}

void OptionsTab::close()
{
	const auto& startOptions = children.begin() + 1;

	children.erase(startOptions, children.end());
	children.resize(1);

	bIsActive = false;
}

void OptionsTab::renderOutlines(std::shared_ptr<Graphics::Renderer> _renderer)
{
	const float pxOutline = 2;
	const SDL_Color outlineColor = { 93, 84, 71, 255 };
	const SDL_Color optionsOutlineColor = { 0, 0, 0, 255 };

	//Outline around entire options tab.
	_renderer->draw_outline(get_position(), get_bounding_rect().get_size(), pxOutline, outlineColor);

	//Render outline around the options header.
	_renderer->draw_outline(get_position(), get_local_rect().get_size(), pxOutline, outlineColor);
	
	if (get_child_count() > 1)
	{
		//Render bounding rectangle around the options specifically.
		_renderer->draw_outline(boundingRectOptions.minPos, boundingRectOptions.get_size(), -pxOutline, optionsOutlineColor);
	}
}

void Option::on_hover()
{
	sprite.color = { 103, 94, 81, 255 };
}

void Option::on_hover_end()
{
	sprite.color = { 93, 84, 71, 255 };
}

void Option::on_left_click()
{
	on_clicked.invoke();
	//TODO: Send Packet with the option args.
	DEVIOUS_EVENT("Selected Option.")
}

void Option::init()
{
	sprite.color = { 93, 84, 71, 255 };
}

void Option::set_option(OptionArgs _args)
{
	args = _args;

	//Define Text
	TextArgs args;
	{
		args.color = { 255, 255, 0 , 255 };
		args.font = e_FontType::RUNESCAPE_UF;
		args.size = 25;
		args.bDropShadow = true;
	}

	//Create visuals
	{
		const float pxWhiteSpacing = 5.0f;
		const float textSpacing = 10.0f;

		std::shared_ptr<TextComponent> textComponent;

		float prevHorTextSize = 0.0f;
		//Create Action Text
		{
			textComponent = TextComponent::create_text(_args.actionStr.c_str(), get_position(), args);
			textComponent->set_anchor(e_AnchorPreset::CENTER_LEFT);

			const float ySizeDiff = parent->get_size().y - textComponent->get_size().y;
			const Utilities::vec2 position = textComponent->get_position();
			textComponent->set_position(position + Utilities::vec2(pxWhiteSpacing, ySizeDiff / 2.0f));

			prevHorTextSize = textComponent->get_size().x + textSpacing;

		}	add_child(textComponent);

		//Create Subject Text
		{
			args.color = { 255, 0, 0, 255 };

			textComponent = TextComponent::create_text(_args.subjectStr.c_str(), get_position(), args);
			textComponent->set_anchor(e_AnchorPreset::CENTER_LEFT);

			const float ySizeDiff = parent->get_size().y - textComponent->get_size().y;
			const Utilities::vec2 position = textComponent->get_position();
			textComponent->set_position(position + Utilities::vec2(prevHorTextSize, ySizeDiff / 2.0f));

		}	add_child(textComponent);
	}
}
