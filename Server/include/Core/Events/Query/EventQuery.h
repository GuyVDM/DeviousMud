#pragma once
#include "Shared/Network/Packets/Packets.hpp"

#include <memory>

using EventStack = std::vector<std::unique_ptr<Packets::s_PacketHeader>>;

class EventQuery
{
public:
	const uint8_t MAX_EVENT_COUNT = 10;

	/// <summary>
	/// Queue a packet ontop of a imaginary stack, only 1 packet of each event type can exist.
	/// If the event type is already apparent within the stack then that event gets replaced by
	/// the latest one.
	/// </summary>
	void queue_packet(std::unique_ptr<Packets::s_PacketHeader> _packet);

	/// <summary>
	/// Retrieve the next packet within the queue whilest
	/// simultaneously removing it from the existing queue. 
	/// </summary>
	std::unique_ptr<Packets::s_PacketHeader> retrieve_next();

	/// <summary>
	/// Returns whether the event query still has packets that need to get handled.
	/// </summary>
	const bool contains_packets() const;

	/// <summary>
	/// Cleans up all remaining events.
	/// </summary>
	void clear();

	/// <summary>
	/// Move all the events to another eventquery.
	/// </summary>
	/// <param name=""></param>
	void move(EventQuery* _target);

public:
	EventQuery() = default;
	~EventQuery() = default;

private:
	//Move function
	EventQuery(EventQuery* _previous);

private:
	EventStack packets;
};

