#include "precomp.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "Config/Config.h"

float App::Config::Configuration::s_DeltaTime;

std::chrono::time_point<std::chrono::high_resolution_clock> App::Config::Configuration::s_LastTime;

float App::Config::Configuration::GetDT()
{
	return s_DeltaTime;
}

void App::Config::Configuration::UpdateDT()
{
	const float MAX_DELTATIME = 0.1f;

	const auto current_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - s_LastTime);
	s_LastTime = current_time;

	const float s = static_cast<float>(duration.count() / 1000000.0f);

	s_DeltaTime = std::min(s, MAX_DELTATIME);
}
