#include "precomp.h"
#include "EventQuery.h"

void EventQuery::queue_packet(std::unique_ptr<Packets::s_PacketHeader> _packet)
{
    e_PacketInterpreter interpreter = _packet->interpreter;

    const auto remove_lower_prio_packets = [this](const e_Action _action)
    {
        for(int i = m_packets.size() - 1; i >= 0; --i) 
        {
            e_Action packetAction = m_packets[i]->action;

            if(packetAction < _action)
            {
                m_packets.erase(m_packets.begin() + i);
            }
        }
    };

    // Make sure that only the highest priority packets remain.
    // This allows us to cancel actions that are unwanted or for some to take priority.
    {
        const e_Action highestPrio = get_highest_packet_priority();

        if (_packet->action > highestPrio)
        {
            remove_lower_prio_packets(_packet->action);
        }
        else if (_packet->action < highestPrio)
        {
            return;
        }
    }

    /// We want only 1 action of a certain type, we check through the currently 
    /// queued packets to see if there's a similar action, if so, overwrite that one.
    {
        auto it = std::find_if(m_packets.begin(), m_packets.end(), [&interpreter](std::unique_ptr<Packets::s_PacketHeader>& _p)
        {
            return interpreter == _p->interpreter;
        });

        if (it != m_packets.end())
        {
            //If there's a packet with the same event, replace it, we only want 1 of each event type.
            //TODO: In the future, i want some events to be whitelisted to happen multiple times within this queue
            auto index = std::distance(m_packets.begin(), it);
            m_packets[index].reset();
            m_packets[index] = std::move(_packet);
            return;
        }
    }

    //Add the event if it doesn't exist yet.
    m_packets.push_back(std::move(_packet));
}

std::unique_ptr<Packets::s_PacketHeader> EventQuery::retrieve_next()
{
    const size_t remaining_event_count = m_packets.size() - 1;

    Packets::s_PacketHeader* packet = m_packets[remaining_event_count].release();
    m_packets.resize(remaining_event_count);

	return std::unique_ptr<Packets::s_PacketHeader>(packet);
}

const e_Action EventQuery::get_highest_packet_priority() const
{
    e_Action highestAction = e_Action::SOFT_ACTION;

    for (const auto& packet : m_packets)
    {
        if (packet->action > highestAction)
        {
            highestAction = packet->action;

            if (highestAction == e_Action::HARD_ACTION)
                break;
        }
    }

    return highestAction;
}

const bool EventQuery::contains_packets() const
{
    return m_packets.size() > 0;
}

void EventQuery::clear()
{
    m_packets.clear();
}

void EventQuery::move(EventQuery* query) 
{
    for(auto& packet : m_packets) 
    {
        query->queue_packet(std::move(packet));
    }

    clear();
}