#pragma once
#include <memory>

#pragma region FORWARD_DECLARATIONS
class Camera;
class ENetPacketHandler;
class EntityHandler;

namespace Graphics
{
	class Renderer;
}

#pragma endregion

/// <summary>
/// Client specific globals, these include systems that are explicitly needed by subsystems in order to function.
/// </summary>
typedef struct _Globals 
{
	std::weak_ptr<Graphics::Renderer> m_renderer;
	std::weak_ptr<ENetPacketHandler>  packetHandler;
	std::weak_ptr<EntityHandler>      m_entityHandler;
}   Globals;

extern Globals g_globals;
