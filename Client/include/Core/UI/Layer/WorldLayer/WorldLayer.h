#pragma once
#include "Core/UI/Layer/Layer.h"

class WorldTile;

namespace Graphics
{
	namespace UI 
	{
		class WorldLayer : public Layer
		{
		public:
			virtual void init() override;

			virtual void update() override;

			virtual bool handle_event(const SDL_Event* m_event) override;

		public: //Implement Parent class constructor
			using Layer::Layer;
			~WorldLayer() override = default;

		private:
			std::vector<std::shared_ptr<WorldTile>> m_tiles;
		};
	}
}
