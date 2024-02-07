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

			bool handle_event(const SDL_Event* event);

			void subscribe_to_event(EventReceiver* receiver);

		public:
			explicit Layer();
			virtual ~Layer() = default;

		protected:
			std::vector<std::shared_ptr<EventReceiver>> receivers;
			std::weak_ptr<Graphics::Renderer> renderer;
		};
	}
}
