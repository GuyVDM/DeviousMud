#pragma once
#include "Core/UI/Layer/Layer.h"

#include "Core/Rendering/Sprite/Sprite.h"

class Canvas;

enum class e_UIInteractionType 
{
	INTERACT          = 0,
	DISPLAY           = 1,
	MOVE              = 2
};

namespace Graphics
{
	namespace UI
	{
		class HUDLayer final : public Layer
		{
		public:
			static e_UIInteractionType get_interaction_type();

			virtual void init() override;

			virtual bool handle_event(const SDL_Event* _event) override;
			
			virtual void update() override;

		public:
			using Layer::Layer;
			virtual ~HUDLayer();

		private:
			/// <summary>
			/// Creates the main UI HUD for the client.
			/// </summary>
			void create_hud();

		private:
			std::shared_ptr<Canvas> m_canvas;

			static e_UIInteractionType s_InteractionType;
		};
	}
}