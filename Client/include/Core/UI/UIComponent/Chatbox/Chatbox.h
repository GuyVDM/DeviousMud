#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

struct ChatboxMessage
{
	std::string name    = "null";
	std::string message = "";
};

class Chatbox : public UIComponent 
{
public:
	static EventListener<ChatboxMessage> s_on_message_received;

public:
	using UIComponent::UIComponent;

	virtual ~Chatbox();

protected:
	virtual void init() override;

	virtual bool handle_event(const SDL_Event* _event) override;


private:
	void update_text();

	void send_message() const;

	void receive_message(ChatboxMessage _message);

private:
	DM::Utils::UUID m_message_received_UUID = 0;

	const int32_t   m_maxCharacterCount = 60;
	std::string     m_inputField        = "";
};