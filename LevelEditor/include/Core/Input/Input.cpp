#include "precomp.h"

#include "Core/Input/Input.h"

#include "Core/Config/Time/Time.hpp"

Ref<Input> g_Input = std::make_shared<Input>();

const bool Input::GetMouseDown(const e_MouseButton& _button) const
{
	if(m_MouseInput.find(_button) == m_MouseInput.end()) 
	{
		return false;
	}

	const Ref<InputContainer> container = m_MouseInput.at(_button);
	if (!container->bIsActive)
	{
		return false;
	}

	return !container->bIsReleased && container->bIsInitialPress;
}

const bool Input::GetMouse(const e_MouseButton& _button) const
{
	if (m_MouseInput.find(_button) == m_MouseInput.end())
	{
		return false;
	}

	const Ref<InputContainer> container = m_MouseInput.at(_button);
	if (!container->bIsActive)
	{
		return false;
	}

	return !container->bIsReleased;
}

const bool Input::GetMouseUp(const e_MouseButton& _button) const
{
	if (m_MouseInput.find(_button) == m_MouseInput.end())
	{
		return false;
	}

	const Ref<InputContainer> container = m_MouseInput.at(_button);
	if (!container->bIsActive)
	{
		return false;
	}

	return container->bIsReleased;
}

const bool Input::GetKeyDown(const SDL_KeyCode& _keyCode) const
{
	if (m_KeyCodes.find(_keyCode) == m_KeyCodes.end())
	{
		return false;
	}

	const Ref<InputContainer> container = m_KeyCodes.at(_keyCode);
	if (!container->bIsActive)
	{
		return false;
	}

	return !container->bIsReleased && container->bIsInitialPress;
}

const bool Input::GetKey(const SDL_KeyCode& _keyCode) const
{
	if (m_KeyCodes.find(_keyCode) == m_KeyCodes.end())
	{
		return false;
	}

	const Ref<InputContainer> container = m_KeyCodes.at(_keyCode);
	if(!container->bIsActive) 
	{
		return false;
	}

	return !container->bIsReleased;
}

const bool Input::GetKeyUp(const SDL_KeyCode& _keyCode) const
{
	if (m_KeyCodes.find(_keyCode) == m_KeyCodes.end())
	{
		return false;
	}

	const Ref<InputContainer> container = m_KeyCodes.at(_keyCode);
	if (!container->bIsActive)
	{
		return false;
	}

	return container->bIsReleased;
}

void Input::SetKeyState(const SDL_KeyCode& _keyCode, const bool& _bIsPressed)
{
	if (_bIsPressed)
	{
		if (m_KeyCodes.find(_keyCode) == m_KeyCodes.end())
		{
			m_KeyCodes[_keyCode] = CreateInputContainer();
		}

		m_KeyCodes[_keyCode]->bIsActive = true;
		m_KeyCodes[_keyCode]->bIsInitialPress = true;
	}
	else
	{
		m_KeyCodes[_keyCode]->bIsReleased = true;
	}
}

void Input::SetMouseState(const e_MouseButton& _mouseButton, const bool& _bIsPressed)
{
	if(_bIsPressed) 
	{
		if (m_MouseInput.find(_mouseButton) == m_MouseInput.end())
		{
			m_MouseInput[_mouseButton] = CreateInputContainer();
		}

		m_MouseInput[_mouseButton]->bIsActive = true;
		m_MouseInput[_mouseButton]->bIsInitialPress = true;
	}
	else 
	{
		m_MouseInput[_mouseButton]->bIsReleased = true;
	}
}

void Input::Update()
{
	UpdateInputStates();
}

void Input::UpdateInputStates()
{
	for(auto&[type, input] : m_MouseInput) 
	{
		UpdateInput(input);

		if(input->bIsReleased) 
		{
			input->Reset();
		}
	}

	for (auto&[type, input] : m_KeyCodes)
	{
		UpdateInput(input);

		if (input->bIsReleased)
		{
			input->Reset();
		}
	}
}

void Input::UpdateInput(Ref<InputContainer> _c)
{
	if (_c->bIsInitialPress)
	{
		_c->bIsInitialPress = false;
	}
}

Ref<InputContainer> Input::CreateInputContainer()
{
	return std::make_shared<InputContainer>();
}
