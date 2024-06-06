#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/UI/UIComponent/Chatbox/Chatbox.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/Entity/EntityHandler.h"

EventListener<ChatboxMessage> Chatbox::s_on_message_received;
EventListener<std::string>    Chatbox::s_on_name_changed;

Chatbox::~Chatbox()
{
    s_on_message_received.remove_listener(m_message_received_UUID);
    s_on_name_changed.remove_listener(m_name_changed_UUID);
}

void Chatbox::init()
{
	m_bIsMovable = true;
    
    //*-------------------------------------
    // Bind callback for receiving messages and name changing.
    //*
    {
        m_message_received_UUID = s_on_message_received.add_listener
        (
            std::bind(&Chatbox::receive_message, this, std::placeholders::_1)
        );

        m_name_changed_UUID = s_on_name_changed.add_listener
        (
            std::bind(&Chatbox::name_changed, this, std::placeholders::_1)
        );
    }

    //*
    // Create input field visual representation.
    //*
    {
        Utilities::vec2 offset = Utilities::vec2(15.0f, -30.0f);
        Utilities::vec2 inputfieldPos = (get_position() + Utilities::vec2(0.0f, get_size().y)) + offset;

        //Create input field.
        {
            TextArgs args;
            args.bDropShadow = false;
            args.color = { 0, 0, 0, 255 };
            args.font = e_FontType::RUNESCAPE_UF;
            args.size = 15;


            auto inputField = Graphics::TextComponent::create_text("", inputfieldPos, args);
            inputField->set_anchor(e_AnchorPreset::BOTTOM_LEFT);
            inputField->set_render_mode(e_RenderMode::SCREENSPACE);
            add_child(inputField);
        }

        //Set initial text.
        update_text();
    }
}

bool Chatbox::handle_event(const SDL_Event* _event)
{
    switch (_event->type)
    {
        case SDL_KEYDOWN:
        {
            switch(_event->key.keysym.sym) 
            {
                case SDLK_RETURN:
                {
                    if (!m_inputField.empty())
                    {
                        send_message();
                        m_inputField.clear();
                        update_text();
                    }
                }
                break;

                case SDLK_BACKSPACE:
                {
                    if (!m_inputField.empty())
                    {
                        m_inputField.pop_back();
                        update_text();
                    }
                }
                break;
            }
        }
        break;

        case SDL_TEXTINPUT:
        {
            if(m_inputField.size() < m_maxCharacterCount) 
            {
                m_inputField.append(_event->text.text);
                update_text();
            }
        }
        break;
    }

	return UIComponent::handle_event(_event);
}

void Chatbox::send_message()
{
    //*-------------------------------------------------------------------
    // Force first character to Uppercase if it's alphanumerical and lower.
    //*
    {
        char c = m_inputField[0];

        if(std::isalpha(c)) 
        {
            if(std::islower(c)) 
            {
                m_inputField[0] = std::toupper(c);
            }
        }
    }

    Packets::s_Message packet;
    packet.interpreter = e_PacketInterpreter::PACKET_ENTITY_MESSAGE_WORLD;
    packet.action      = e_Action::SOFT_ACTION;
    packet.entityId    = g_globals.entityHandler.lock()->get_local_player_id();
    packet.message     = m_inputField;
    packet.author      = "Player";

    g_globals.packetHandler.lock()->send_packet<Packets::s_Message>
    (
        &packet,
        0, 
        ENET_PACKET_FLAG_RELIABLE
    );
}

void Chatbox::receive_message(ChatboxMessage _message)
{
    TextArgs args;
    {
        args.bDropShadow = false;
        args.color = { 0, 0, 0, 255 };
        args.font = e_FontType::RUNESCAPE_UF;
        args.size = 15;
    }

    std::string chatboxMessage = "";

    //*
    // Construct final message before we turn it into an object.
    //*
    {
        if (_message.name != "")
        {
            chatboxMessage.append(_message.name + ": <col=#1b19ed>");
        }

        chatboxMessage.append(_message.message);
    }

    const int32_t maxMessageCount = 8;

    auto messageItem = Graphics::TextComponent::create_text(chatboxMessage, Utilities::vec2(0.0f), args);
    {
        if(get_child_count() - 1 >= maxMessageCount) 
        {
            const int32_t index = get_child_count() - 1;
            m_children[index]->unparent();
        }

        messageItem->set_anchor(e_AnchorPreset::BOTTOM_LEFT);
        messageItem->set_render_mode(e_RenderMode::SCREENSPACE);
        add_child(messageItem, 1);

        for(int32_t i = 1; i < m_children.size(); i++) 
        {
            auto& msg = m_children[i];
            const Utilities::vec2 startLocation = Utilities::vec2(15.0f, -34.0f);
            const Utilities::vec2 offsetY = Utilities::vec2(0, -17.0f) * static_cast<float>(i);
            const Utilities::vec2 inputfieldPos = (get_position() + Utilities::vec2(0.0f, get_size().y)) + startLocation + offsetY;
            msg->set_position(inputfieldPos);

        }
    }
}

void Chatbox::name_changed(const std::string _name)
{
    m_playerName = _name;
    update_text();
}

void Chatbox::update_text()
{
    const std::string marker = "*";

    std::string text = m_playerName;
    text.append(":  <col=#1b19ed>");
    text.append(m_inputField);
    text.append(marker);

    auto inputField = std::static_pointer_cast<Graphics::TextComponent>(m_children[0]);
    inputField->set_text(text);
}
