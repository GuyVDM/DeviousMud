#pragma once
#include "Core/Core.hpp"

#include <string>

#include <vector>

enum e_LogType : U8
{
	TYPE_INFO  = 1 << 0,
	TYPE_EVENT = 1 << 1,
	TYPE_WARN  = 1 << 2,
	TYPE_ERROR = 1 << 3
};

class StringWrapper 
{
public:
	inline StringWrapper()
		: m_String("") {}

	inline StringWrapper(const char* _s)
		: m_String(_s) {}

	inline StringWrapper(const std::string& _s)
		: m_String(_s) {}

	inline StringWrapper& Append(const char* _s)
	{
		m_String += _s;
		return *this;
	}

	inline StringWrapper& Append(const std::string& _s)
	{
		m_String += _s;
		return *this;
	}

	inline const std::string& GetString() const
	{
		return m_String;
	}

	template<typename T>
	inline StringWrapper& operator<<(const T& _value)
	{
		return Append(_value);
	}

	template<typename T>
	inline StringWrapper& Append(const T& _value)
	{
		std::ostringstream oss;
		oss << _value;
		m_String += oss.str();
		return *this;
	}

private:
	std::string m_String;
};


struct Message 
{
	std::string Contents = "";
	e_LogType   LogType  = e_LogType::TYPE_INFO;
};

class Logger 
{
public:
	static const U32 MAX_LOG_COUNT = 999;

	inline static void Log(StringWrapper _string, const e_LogType _type) 
	{
		std::string contents;

		switch(_type) 
		{
			case e_LogType::TYPE_INFO:
			{
				contents = "[Info] ";
			}
			break;

			case e_LogType::TYPE_WARN:
			{
				contents = "[Warn] ";
			}
			break;

			case e_LogType::TYPE_ERROR:
			{
				contents = "[Error] ";
			}
			break;

			case e_LogType::TYPE_EVENT:
			{
				contents = "[Event] ";
			}
			break;
		}

		contents.append(_string.GetString());
		contents.append("\n");

		if(s_Messages.size() >= Logger::MAX_LOG_COUNT) 
		{
			s_Messages.erase(s_Messages.begin());
		}

		Message message = Message(contents, _type);
		s_Messages.push_back(message);

#ifdef _DEBUG
		std::cout << message.Contents;
#endif
	}

	inline static std::vector<Message>& GetMessages() 
	{
		return s_Messages;
	}

	inline static U32 GetMessagesCountOfLogType(const e_LogType& _logType)
	{
		return std::accumulate(s_Messages.begin(), s_Messages.end(), 0, 
			[_logType](I32 _count, const Message& _message)
			{
				return _count + (_message.LogType == _logType ? 1 : 0);
			}
		);
	}

	inline static void ClearLog() 
	{
		s_Messages.clear();
	}

private:
	inline static std::vector<Message> s_Messages;
};

#define DEVIOUS_LOG(msg) Logger::Log(StringWrapper() << msg, e_LogType::TYPE_INFO);
#define DEVIOUS_ERR(msg) Logger::Log(StringWrapper() << msg, e_LogType::TYPE_ERROR);
#define DEVIOUS_WARN(msg) Logger::Log(StringWrapper() << msg, e_LogType::TYPE_WARN);
#define DEVIOUS_EVENT(msg) Logger::Log(StringWrapper() << msg, e_LogType::TYPE_EVENT);