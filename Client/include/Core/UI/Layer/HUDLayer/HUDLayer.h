#pragma once
#include "Core/UI/Layer/Layer.h"

#include "Core/Rendering/Sprite/Sprite.h"

#include "Core/UI/UIComponent/HUDTab/HUDTabType.h"

#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/vec2.hpp"

class HUDTab;

namespace Graphics
{
	namespace UI
	{
		struct UIElement
		{
			Utilities::vec2  position;
			Utilities::vec2  scale;
			Graphics::Sprite sprite;

		};

		class HUDLayer final : public Layer
		{
		public:
			EventListener<e_TabType> on_selected;

			virtual void init() override;

			virtual bool handle_event(const SDL_Event* event) override;
			
			virtual void update() override;

		public:
			using Layer::Layer;
			~HUDLayer() override = default;

		private:
			//vec2 get_hud_size();

			/// <summary>
			/// Creates the main UI HUD for the client.
			/// </summary>
			void create_hud();

			/// <summary>
			/// Opens the corresponding menu based on the tab clicked.
			/// </summary>
			/// <param name="_tab"></param>
			void open_menu(e_TabType _tab);

		private:
			std::vector<UIElement>               elements;
			std::vector<std::shared_ptr<HUDTab>> tabs;
		};
	}
}