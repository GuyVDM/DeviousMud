#pragma once
#include <memory>

#pragma region FORWARD_DECLARATIONS
class Camera;
class ENetPacketHandler;
class EntityHandler;
class TimerHandler;

namespace Graphics
{
	class Renderer;
}

#pragma endregion

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
struct Globals
{
	std::weak_ptr<Graphics::Renderer> renderer;
	std::weak_ptr<ENetPacketHandler>  packetHandler;
	std::weak_ptr<EntityHandler>      entityHandler;
	std::weak_ptr<TimerHandler>       timerHandler;
};

extern Globals g_globals;
