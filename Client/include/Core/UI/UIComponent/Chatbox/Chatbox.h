#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

class Chatbox : public UIComponent 
{
public:
	using UIComponent::UIComponent;

	virtual ~Chatbox();

protected:
	virtual void init() override;

	virtual bool handle_event(const SDL_Event* _event) override;

	void send_message() const;

private:
	void update_text();

private:
	const int32_t m_maxCharacterCount = 50;
	std::string m_inputField = "";
};