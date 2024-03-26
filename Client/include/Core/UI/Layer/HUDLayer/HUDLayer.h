#pragma once
#include "Core/UI/Layer/Layer.h"

#include "Core/Rendering/Sprite/Sprite.h"

class UIComponent;
class Canvas;

namespace Graphics
{
	namespace UI
	{
		class HUDLayer final : public Layer
		{
		public:
			virtual void init() override;

			virtual bool handle_event(const SDL_Event* event) override;
			
			virtual void update() override;

		public:
			using Layer::Layer;
			virtual ~HUDLayer() = default;

		private:
			/// <summary>
			/// Creates the main UI HUD for the client.
			/// </summary>
			void create_hud();

		private:
			std::shared_ptr<Canvas> canvas;
		};
	}
}