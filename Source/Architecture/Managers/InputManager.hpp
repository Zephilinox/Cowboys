#pragma once

//STD
#include <array>
#include <mutex>
#include <string>
#include <map>
#include <experimental/map>

//LIB
#include <Engine/Keys.h>
#include <Engine/Input.h>

namespace ASGE
{
	namespace KEYS
	{
		/**< Extra key defines */
		constexpr int KEY_F1 = 290;
		constexpr int KEY_F2 = 291;
		constexpr int KEY_F3 = 292;
		constexpr int KEY_F4 = 293;
		constexpr int KEY_LAST = 512; /**< GLFW defines it as 348 however I want to be safe. */
	}
}

/**
* Input Manager. Handles input thread safety and different key states.
*/

/*
The input manager has some issues but I'm leaving it as it is for the time being.
Use isKeyDown over isKeyPressed if at all possible.
*/
class InputManager
{
public:
	InputManager(ASGE::Input* input) noexcept;
	~InputManager();

	void update();
	void handleInput(int key, int state);

	void addAction(std::string action, unsigned id);
	bool removeAction(std::string action, unsigned id);

	bool isActionPressed(std::string action);
	bool isActionDown(std::string action);

	bool isKeyPressed(int key);
	bool isKeyDown(int key) noexcept;

	bool isGamePadButtonPressed(int button);
	bool isGamePadButtonDown(int button);

	GamePadData getGamePad();

	int gamepad_button_up = ASGE::KEYS::KEY_LAST + 10;
	int gamepad_button_down = ASGE::KEYS::KEY_LAST + 12;
	int gamepad_button_enter = ASGE::KEYS::KEY_LAST + 0;
	int gamepad_button_escape = ASGE::KEYS::KEY_LAST + 7;

private:
	void gamepadHandler(const ASGE::SharedEventData data);

	int gamepad_id;
	bool gamepad_connected = false;
	int callback_id;

	ASGE::Input* input;
	std::mutex keys_mutex;
	
	std::multimap<std::string, int> actions;

	std::array<int, ASGE::KEYS::KEY_LAST> toggle_keys;
	std::array<int, ASGE::KEYS::KEY_LAST> keys;

	std::array<int, ASGE::KEYS::KEY_LAST + ASGE::KEYS::KEY_LAST> buttons_last_frame;
	std::array<int, ASGE::KEYS::KEY_LAST + ASGE::KEYS::KEY_LAST> buttons;
};
