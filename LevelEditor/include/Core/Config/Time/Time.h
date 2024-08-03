#pragma once
#include "Core/Core.h"

#include <chrono>

class Time
{
public:
	inline static float GetDeltaTime()
	{
		return s_DeltaTime;
	}

	inline static float GetElapsedTime()
	{
		return s_ElapsedTime;
	}

	inline static void  Update() 
	{
		const float MAX_DELTATIME = 0.1f;

		const auto current_time = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - s_LastTime);
		s_LastTime = current_time;

		const float s = static_cast<float>(duration.count() / 1000000.0f);

		s_DeltaTime = std::min(s, MAX_DELTATIME);
		s_ElapsedTime += s_DeltaTime;
	}

private:
	inline static float s_ElapsedTime = 0.0f;
	inline static float s_DeltaTime   = 0.0f;
	inline static std::chrono::time_point<std::chrono::high_resolution_clock> s_LastTime;
};