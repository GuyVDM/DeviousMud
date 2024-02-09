#pragma once
#include "Core/UI/Layer/Layer.h"
#include "Core/Rendering/Sprite/Sprite.h"

#pragma region FORWARD_DECLERATIONS
class PlayerHandler;

class Camera;

namespace Graphics 
{
	class Renderer;
}
#pragma endregion

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

			void player_local_assigned(uint64_t _playerId);

		private:
			bool bHasLocalPlayer = false;

			std::shared_ptr<Camera>		        playerCamera;

			std::shared_ptr<PlayerHandler>      playerHandler;

			std::shared_ptr<Graphics::Renderer> renderer;

			std::vector<uint64_t>               playerHandles;

			Graphics::Sprite                    playerSprite;

		};
	}
}