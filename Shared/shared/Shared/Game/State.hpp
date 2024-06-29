#pragma once
#include <memory>

#define IMPLEMENT_STATE_METHODS \
public: \
State::State; \
virtual void on_state_start() override; \
virtual void on_state_update() override; \
virtual void on_state_end() override;

namespace DM
{
	namespace State
	{
		/// <summary>
		/// Generic state machine implementation.
		/// </summary>
		/// <typeparam name="T">The object the state governs.</typeparam>
		template<class T>
		class State
		{
		public:
			virtual void on_state_start() {};

			virtual void on_state_update() {};

			virtual void on_state_end() {};

			State(std::shared_ptr<T> _owner) : m_owner(_owner) {};
			State() {};
			virtual ~State() = default;

		protected:
			std::weak_ptr<T> m_owner = nullptr;
		};
	}
}