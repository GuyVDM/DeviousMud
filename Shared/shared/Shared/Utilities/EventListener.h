#pragma once
#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H

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
	void add_listener(Listener _listener);

	virtual void invoke(Parameter _param);

	void clear();

private:
	std::vector<Listener> listeners;
};

template<typename Parameter>
inline void EventListener<Parameter>::add_listener(Listener _listener) 
{
	listeners.push_back(_listener);
}

template<typename Parameter>
inline void EventListener<Parameter>::invoke(Parameter _param)
{
	for(const auto& listener : listeners) 
	{
		listener(_param);
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
	inline void add_listener(Listener _listener)
	{
		listeners.push_back(_listener);
	}

	inline void invoke() 
	{
		for (const auto& listener : listeners)
		{
			listener();
		}
	}

	inline void clear()
	{
		listeners.clear();
	}

private:
	std::vector<Listener> listeners;
};


#pragma endregion
#endif
