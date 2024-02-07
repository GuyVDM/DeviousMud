#pragma once
#include "Core/UI/Layer/Layer.h"

namespace Graphics
{
	namespace UI 
	{
		class WorldLayer : public Layer
		{
		public: //Implement Parent class constructor
			using Layer::Layer;
			~WorldLayer() override = default;

		public :
			virtual void init() override;
		};
	}
}
