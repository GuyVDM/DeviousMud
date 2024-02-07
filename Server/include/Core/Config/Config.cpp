#include "precomp.h"
#include "Config.h"

#include <cmath>

//Declaration of static variable deltatime.
float DEVIOUSMUD::SERVER::Config::deltaTime;

//Decleration of last time tracked.
std::chrono::time_point<std::chrono::high_resolution_clock> DEVIOUSMUD::SERVER::Config::last_time;

//Return the last updated deltatime.
float DEVIOUSMUD::SERVER::Config::get_deltaTime()
{
	return deltaTime;
}

/// <summary>
/// Update the deltatime to represent the duration between the previous and current frame.
/// Only call this once per start of the frame.
/// </summary>
void DEVIOUSMUD::SERVER::Config::update_deltaTime()
{
	const float MAX_DELTATIME = 0.1f;

	const auto current_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time);
	last_time = current_time;

	const float s = static_cast<float>(duration.count() / 1000000.0f);

	deltaTime = std::min(s, MAX_DELTATIME);
}
