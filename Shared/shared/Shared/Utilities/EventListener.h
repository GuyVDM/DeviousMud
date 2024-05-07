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
	std::unordered_map<DM::Utils::UUID, Listener> m_listeners;
};

template<typename Parameter>
inline DM::Utils::UUID EventListener<Parameter>::add_listener(Listener _listener)
{
	DM::Utils::UUID uuid = DM::Utils::UUID::generate();
	m_listeners[uuid] = _listener;
	return uuid;
}

template<typename Parameter>
inline void EventListener<Parameter>::invoke(Parameter _param)
{
	for (const auto& listener : m_listeners)
	{
		auto& function = listener.second;
		function(_param);
	}
}

template<typename Parameter>
inline void EventListener<Parameter>::clear()
{
	m_listeners.clear();
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
		m_listeners[uuid] = _listener;
		return uuid;
	}

	inline void invoke() 
	{
		for (const auto& listener : m_listeners)
		{
			auto& function = listener.second;
			function();
		}
	}

	inline void remove_listener(DM::Utils::UUID _id) 
	{
		if(m_listeners.find(_id) != m_listeners.end()) 
		{
			m_listeners.erase(_id);
		}
	}

	inline void clear()
	{
		m_listeners.clear();
	}

private:
	std::unordered_map<DM::Utils::UUID, Listener> m_listeners;
};


#pragma endregion
#endif
