#include "precomp.h"
#include "EventQuery.h"

void EventQuery::queue_packet(std::unique_ptr<Packets::s_PacketHeader> _packet)
{
    e_PacketInterpreter interpreter = _packet->interpreter;

    auto it = std::find_if(packets.begin(), packets.end(), [&interpreter](std::unique_ptr<Packets::s_PacketHeader>& _p)
        {
        return interpreter == _p->interpreter;
        });

    if (it != packets.end()) 
    {
        //If there's a packet with the same event, replace it, we only want 1 of each event type.
        //TODO: In the future, i want some events to be whitelisted to happen multiple times within this queue
        auto index = std::distance(packets.begin(), it);
        packets[index].reset();
        packets[index] = std::move(_packet);
        return;
    }

    //Add the event if it doesn't exist yet.
    packets.push_back(std::move(_packet));
}

std::unique_ptr<Packets::s_PacketHeader> EventQuery::retrieve_next()
{
    const size_t remaining_event_count = packets.size() - 1;

    Packets::s_PacketHeader* packet = packets[remaining_event_count].release();
    packets.resize(remaining_event_count);

	return std::unique_ptr<Packets::s_PacketHeader>(packet);
}

const bool EventQuery::contains_packets() const
{
    return packets.size() > 0;
}

void EventQuery::move(EventQuery* query) 
{
    for(auto& packet : packets) 
    {
        query->queue_packet(std::move(packet));
    }

    packets.clear();
}