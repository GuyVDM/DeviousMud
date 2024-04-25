#pragma once
#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H

#include "Shared/Utilities/UUID.hpp"

#include <functional>
#include <vector>

/// <summary>
/// Allows for the subscribing of methods which can get called accumutively
/// </summary>
/// <typeparam name="Parameter"></typeparam>
template<typename Parameter>
class EventListener
{
	using Listener = std::function<void(Parameter)>;

public:
	DM::Utils::UUID add_listener(Listener _listener);

	void remove_listener(DM::Utils::UUID _uuid);

	void invoke(Parameter _param);

	void clear();

private:
	std::unordered_map<DM::Utils::UUID, Listener> listeners;
};

template<typename Parameter>
inline DM::Utils::UUID EventListener<Parameter>::add_listener(Listener _listener)
{
	DM::Utils::UUID uuid = DM::Utils::UUID::generate();
	listeners[uuid] = _listener;
	return uuid;
}

template<typename Parameter>
inline void EventListener<Parameter>::invoke(Parameter _param)
{
	for (const auto& listener : listeners)
	{
		auto& function = listener.second;
		function(_param);
	}
}

template<typename Parameter>
inline void EventListener<Parameter>::clear()
{
	listeners.clear();
}

#pragma region VOID_SPECIALIZATION_NO_PARAM
template<>
class EventListener<void>
{
	using Listener = std::function<void()>;

public:
	inline DM::Utils::UUID add_listener(Listener _listener)
	{
		DM::Utils::UUID uuid = DM::Utils::UUID::generate();
		listeners[uuid] = _listener;
		return uuid;
	}

	inline void invoke() 
	{
		for (const auto& listener : listeners)
		{
			auto& function = listener.second;
			function();
		}
	}

	inline void remove_listener(DM::Utils::UUID _id) 
	{
		if(listeners.find(_id) != listeners.end()) 
		{
			listeners.erase(_id);
		}
	}

	inline void clear()
	{
		listeners.clear();
	}

private:
	std::unordered_map<DM::Utils::UUID, Listener> listeners;
};


#pragma endregion
#endif
