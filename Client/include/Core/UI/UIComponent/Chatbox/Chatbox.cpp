#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/UI/UIComponent/Chatbox/Chatbox.h"

#include "Core/Network/Packets/ENetPacketHandler.h"

#include "Core/Global/C_Globals.h"

#include "Core/Entity/EntityHandler.h"

Chatbox::~Chatbox()
{
}

void Chatbox::init()
{
	m_bIsMovable = true;

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

void Chatbox::send_message() const
{
    Packets::s_Message packet;
    packet.interpreter = e_PacketInterpreter::PACKET_ENTITY_MESSAGE_WORLD;
    packet.action      = e_Action::SOFT_ACTION;
    packet.entityId    = g_globals.entityHandler.lock()->get_local_player_id();
    packet.message     = m_inputField;

    g_globals.packetHandler.lock()->send_packet<Packets::s_Message>
    (
        &packet,
        0, 
        ENET_PACKET_FLAG_RELIABLE
    );
}

void Chatbox::update_text()
{
    const std::string namePlaceholder = "Player: ";
    const std::string marker = "*";

    std::string text = namePlaceholder;
    text.append(m_inputField);
    text.append(marker);

    auto inputField = std::static_pointer_cast<Graphics::TextComponent>(m_children[0]);
    inputField->set_text(text);
}
