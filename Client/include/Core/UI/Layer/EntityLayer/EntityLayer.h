#pragma once
#include "Core/UI/Layer/Layer.h"
#include "Core/Rendering/Sprite/Sprite.h"

#pragma region FORWARD_DECLERATIONS
class EntityHandler;

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
		class EntityLayer final : public Layer
		{
		public:
			virtual void init() override;

			virtual void update() override;

		public: //Implement Parent class constructor
			using Layer::Layer;
			~EntityLayer() override = default;

		private:
			void player_created(uint64_t _playerId);

			void player_destroyed(uint64_t _playerId);

			void player_local_assigned(uint64_t _playerId);

		private:
			bool m_bHasLocalPlayer = false;

			Utilities::vec2 m_playerSize = Utilities::vec2(64.0f);

			std::shared_ptr<Camera>		        m_playerCamera;

			std::shared_ptr<EntityHandler>      m_entityHandler;

			std::shared_ptr<Graphics::Renderer> m_renderer;

			std::vector<uint64_t>               m_playerHandles;
		};
	}
}