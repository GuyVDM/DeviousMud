#pragma once
#include <vector>
#include <memory>

#pragma region FORWARD_DECLERATIONS
namespace Graphics 
{
	class Renderer;
}
class EventReceiver;
typedef union SDL_Event SDL_Event;
#pragma endregion

namespace Graphics
{
	class Renderer;

	namespace UI
	{
		class Layer
		{
		public:
			virtual void update();

			virtual void init();

			virtual bool handle_event(const SDL_Event* _event);

		public:
			explicit Layer();
			virtual ~Layer() = default;

		protected:
			std::shared_ptr<Graphics::Renderer> m_renderer;
		};
	}
}
