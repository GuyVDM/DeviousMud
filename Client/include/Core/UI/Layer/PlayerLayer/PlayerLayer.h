#pragma once
#include "Core/UI/Layer/Layer.h"
#include "Core/Rendering/Sprite/Sprite.h"

class PlayerHandler;

namespace Graphics 
{
	class Renderer;
}

namespace Graphics
{
	namespace UI
	{
		class PlayerLayer final : public Layer
		{
		public:
			virtual void init() override;

			virtual void update() override;

		public: //Implement Parent class constructor
			using Layer::Layer;
			~PlayerLayer() override = default;

		private:
			void player_created(uint64_t _playerId);

			void player_destroyed(uint64_t _playerId);

		private:
			std::vector<uint64_t>         playerHandles;

			std::weak_ptr<PlayerHandler> weakPlayerHandler;

			Graphics::Sprite             playerSprite;

		};
	}
}