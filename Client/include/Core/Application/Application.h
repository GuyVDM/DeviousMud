#pragma once
#include <memory>
#include <map>

#include "Core/Rendering/Cursor/RSCross.h"

namespace Graphics 
{
	class Renderer;

	namespace UI
	{
		class Layer;
	}
}

class ENetPacketHandler;
class Camera;

class Application
{
public:
	const bool is_running() const;

	void close_application();

	void init();

	void update();

public:
	Application();
	~Application() = default;

private:
	void load_sprites();

	void create_layers();

private:
	bool bIsrunning;

	std::unique_ptr<RSCross> cursor;
	std::vector<std::unique_ptr<Graphics::UI::Layer>> layers;
};