#include "precomp.h"

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
                    }
                }
                break;

                case SDLK_BACKSPACE:
                {
                    if (!m_inputField.empty())
                    {
                        m_inputField.pop_back();
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
    packet.action      = e_Action::MEDIUM_ACTION;
    packet.entityId    = g_globals.entityHandler.lock()->get_local_player_id();
    packet.message     = m_inputField;

    g_globals.packetHandler.lock()->send_packet<Packets::s_Message>
    (
        &packet,
        0, 
        ENET_PACKET_FLAG_RELIABLE
    );

    DEVIOUS_LOG("Sent: " << packet.message);
}
