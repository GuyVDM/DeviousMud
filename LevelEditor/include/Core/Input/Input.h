#pragma once

#include "Core/Core.hpp"

#include <unordered_map>

enum class e_MouseButton 
{
	NONE   = 0x00,
	BUTTON_LEFT,
	BUTTON_MIDDLE,
	BUTTON_RIGHT
};

enum class e_InputType : U8
{
	TYPE_MOUSE = 0x00,
	TYPE_KEY
};

struct DestroyArgs 
{
	e_InputType     InputType;
	SDL_KeyCode     KeyType;
	e_MouseButton   MouseButton;
};

struct InputContainer
{
	bool  bIsActive       = false;
	bool  bIsInitialPress = false;
	bool  bIsReleased     = false;

	inline InputContainer()
	{
		bIsActive       = true;
		bIsInitialPress = true;
		bIsReleased     = false;
	}

	inline void Reset() 
	{
		bIsActive       = false;
		bIsInitialPress = true;
		bIsReleased     = false;
	}
};

class Input
{
public:
	/// <summary>
	/// Whether this is the first frame the mouse button has been pressed.
	/// </summary>
	/// <param name="_button"></param>
	/// <returns></returns>
	const bool GetMouseDown(const e_MouseButton& _button) const;

	/// <summary>
	/// Returns true if the mouse is kept being held.
	/// </summary>
	/// <param name="_button"></param>
	/// <returns></returns>
	const bool GetMouse(const e_MouseButton& _button) const;

	/// <summary>
	/// Returns true if the mouse button is released.
	/// </summary>
	/// <param name="_button"></param>
	/// <returns></returns>
	const bool GetMouseUp(const e_MouseButton& _button) const;

	/// <summary>
	/// Returns true when it's the first frame after the button has been released.
	/// </summary>
	/// <param name="_keyCode"></param>
	/// <returns></returns>
	const bool GetKeyDown(const SDL_KeyCode& _keyCode) const;

	/// <summary>
	/// Returns true when the specified key is currently repeating.
	/// </summary>
	/// <param name="_keyCode"></param>
	/// <returns></returns>
	const bool GetKey(const SDL_KeyCode& _keyCode) const;

	/// <summary>
	/// Returns true if it's the first frame after the key has been released.
	/// </summary>
	/// <param name="_keyCode"></param>
	/// <returns></returns>
	const bool GetKeyUp(const SDL_KeyCode& _keyCode) const;

public:
	Input()  = default;
    ~Input() = default;

private:
	Input(const Input& _input) = delete;

private:
	void SetKeyState(const SDL_KeyCode& _keyCode, const bool& _bIsPressed);

	void SetMouseState(const e_MouseButton& _mouseButton, const bool& _bIsPressed);

	void Update();

	void UpdateInputStates();

	void UpdateInput(Ref<InputContainer> _c);

	Ref<InputContainer> CreateInputContainer();

private:
	std::unordered_map<SDL_KeyCode,     Ref<InputContainer>> m_KeyCodes;

	std::unordered_map<e_MouseButton, Ref<InputContainer>> m_MouseInput;

	std::vector<DestroyArgs> m_DestroyQueue;

	friend class InputHandler;
};

extern Ref<Input> g_Input;
