/*
* File: InputManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.22
*/

#include "MW.h"

namespace Milkweed {
	InputManager InputManager::m_instance;

	void InputManager::init() {
		// Add the GLFW callback functions
		glfwSetKeyCallback(MW::WINDOW.getWindowHandle(), KeyCallback);
		glfwSetCharCallback(MW::WINDOW.getWindowHandle(), CharCallback);
		glfwSetMouseButtonCallback(MW::WINDOW.getWindowHandle(),
			ButtonCallback);
		glfwSetCursorPosCallback(MW::WINDOW.getWindowHandle(),
			CursorCallback);
		glfwSetScrollCallback(MW::WINDOW.getWindowHandle(),
			ScrollCallback);
		glfwSetJoystickCallback(JoystickCallback);

		// Attempt to connect up to 16 gamepads
		for (unsigned int gp = 0; gp < GLFW_JOYSTICK_LAST; gp++) {
			if (glfwJoystickPresent(gp)) {
				if (glfwJoystickIsGamepad(gp)) {
					JoystickCallback(gp, GLFW_CONNECTED);
				}
			}
		}

		MWLOG(Info, InputManager, "Set GLFW input callback functions for ",
			"keyboard, text, mouse buttons and mouse movement");
	}

	void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode,
		int action, int mods) {
		// Decide whether the key was pressed or released and modify the key
		// in the application's input manager
		switch (action) {
		case GLFW_PRESS:
			// Update the state of the key and notify input listeners
			MW::INPUT.m_keys[key] = true;
			for (InputListener* l : MW::INPUT.m_listeners) {
				l->keyPressed(key);
			}
			break;
		case GLFW_RELEASE:
			// Update the state of the key and notify input listeners
			MW::INPUT.m_keys[key] = false;
			for (InputListener* l : MW::INPUT.m_listeners) {
				l->keyReleased(key);
			}
			break;
		}
	}

	void InputManager::CharCallback(GLFWwindow* window,
		unsigned int codepoint) {
		// Notify input listeners
		for (InputListener* l : MW::INPUT.m_listeners) {
			l->textTyped((char)codepoint);
		}
	}
	
	void InputManager::ButtonCallback(GLFWwindow* window, int button,
		int action, int mods) {
		// Decide whether the button was pressed or released and modify it in
		// the application's input manager
		switch (action) {
		case GLFW_PRESS:
			// Update the state of the button and notify input listeners
			MW::INPUT.m_buttons[button] = true;
			for (InputListener* l : MW::INPUT.m_listeners) {
				l->buttonPressed(button);
			}
			break;
		case GLFW_RELEASE:
			// Update the state of the button and notify input listeners
			MW::INPUT.m_buttons[button] = false;
			for (InputListener* l : MW::INPUT.m_listeners) {
				l->buttonReleased(button);
			}
			break;
		}
	}

	void InputManager::CursorCallback(GLFWwindow* window, double x, double y) {
		// Set the new cursor position and notify input listeners (invert the
		// y-axis)
		for (InputListener* l : MW::INPUT.m_listeners) {
			l->cursorMoved();
		}
	}

	void InputManager::ScrollCallback(GLFWwindow* window, double xOffset,
		double yOffset) {
		for (InputListener* l : MW::INPUT.m_listeners) {
			l->scrolled(glm::vec2(xOffset, yOffset));
		}
	}

	void InputManager::JoystickCallback(int jid, int event) {
		if (event == GLFW_CONNECTED) {
			if (glfwJoystickIsGamepad(jid)) {
				// Attempt to find the gamepad in the list already
				std::unordered_map<int, GLFWgamepadstate>::iterator it
					= MW::INPUT.m_gamepads.find(jid);
				if (it != MW::INPUT.m_gamepads.end()) {
					return;
				}
				// The gamepad can be connected
				MW::INPUT.m_gamepads[jid] = GLFWgamepadstate();
				glfwGetGamepadState(jid, &(MW::INPUT.m_gamepads[jid]));
				MWLOG(Info, InputManager, "Connected gamepad ", jid,
					" with name \"", glfwGetGamepadName(jid), "\"");
			}
		}
	}

	void InputManager::addInputListener(InputListener* listener) {
		// Add the input manager if it is not already present in the list
		if (std::find(m_listeners.begin(), m_listeners.end(), listener)
			== m_listeners.end()) {
			m_listeners.push_back(listener);
		}
	}

	void InputManager::removeInputListener(InputListener* listener) {
		// Call remove on the list with the listener
		m_listeners.remove(listener);
	}
	
	void InputManager::update() {
		// Push the key states into the next frame
		for (const std::pair<int, bool>& k : m_keys) {
			m_prevKeys[k.first] = k.second;
		}
		// Push the button states into the next frame
		for (const std::pair<int, bool>& b : m_buttons) {
			m_prevButtons[b.first] = b.second;
		}
		// Push the gamepad states into the next frame
		m_prevGamepads.clear();
		std::vector<std::unordered_map<int, GLFWgamepadstate>::iterator> removals;
		for (const std::pair<int, GLFWgamepadstate>& g : m_gamepads) {
			if (glfwJoystickPresent(g.first)) {
				m_prevGamepads[g.first] = g.second;
			}
			else {
				// The gamepad is not connected, remove it from both maps if
				// present.
				MWLOG(Info, InputManager, "Disconnected gamepad ", g.first);
				std::unordered_map<int, GLFWgamepadstate>::iterator it
					= m_gamepads.find(g.first);
				if (it != m_gamepads.end()) {
					removals.push_back(it);
				}
			}
		}
		// Remove all disconnected gamepads from the list
		for (std::unordered_map<int, GLFWgamepadstate>::iterator it : removals) {
			m_gamepads.erase(it);
		}

		// Check GLFW for new events
		glfwPollEvents();
		// Check GLFW for new gamepad events
		for (const std::pair<int, GLFWgamepadstate>& g : m_gamepads) {
			glfwGetGamepadState(g.first, &(m_gamepads[g.first]));
		}
		processGamepadInput();

		// Update the cursor position
		double x = 0.0, y = 0.0;
		glfwGetCursorPos(MW::WINDOW.getWindowHandle(), &x, &y);
		m_cursorPosition.x = (float)x;
		m_cursorPosition.y = MW::WINDOW.getDimensions().y - (float)y;
	}

	void InputManager::processGamepadInput() {
		for (const std::pair<int, GLFWgamepadstate>& g : m_gamepads) {
			// Attempt to find the previous frame's data for this gamepad
			std::unordered_map<int, GLFWgamepadstate>::iterator it
				= m_prevGamepads.find(g.first);
			if (it == m_prevGamepads.end()) {
				continue;
			}
			// The gamepad was present last frame
			// Check all buttons for updates since the last frame
			for (unsigned int button = 0; button <= GLFW_GAMEPAD_BUTTON_LAST;
				button++) {
				if (m_gamepads[g.first].buttons[button]
					!= m_prevGamepads[g.first].buttons[button]) {
					// The button was changed
					bool pressed = (m_gamepads[g.first].buttons[button]
						== GLFW_PRESS) ? true : false;
					for (InputListener* l : m_listeners) {
						if (pressed) {
							l->gamepadButtonPressed(g.first, button);
						}
						else {
							l->gamepadButtonReleased(g.first, button);
						}
					}
				}
			}
			// Check all axes for updates since the last frame
			for (unsigned int axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; axis++) {
				if (m_gamepads[g.first].axes[axis]
					!= m_prevGamepads[g.first].axes[axis]) {
					for (InputListener* l : m_listeners) {
						l->gamepadAxisMoved(g.first, axis);
					}
				}
			}
		}
	}

	bool InputManager::isKeyDown(int key) {
		// Attempt to find the key in the key map
		std::unordered_map<int, bool>::iterator it = m_keys.find(key);
		if (it == m_keys.end()) {
			// The key was not found, it is not down
			return false;
		}
		// Give the state of the key
		return it->second;
	}

	bool InputManager::isKeyPressed(int key) {
		// If the key is not down it can't be pressed
		if (!isKeyDown(key)) {
			return false;
		}

		// The key is down, attempt to find it in the previous frame's map
		std::unordered_map<int, bool>::iterator it = m_prevKeys.find(key);
		if (it == m_prevKeys.end()) {
			// The key is not in the previous frame, it has just been pressed
			return true;
		}
		
		// Since the key is down, it has been pressed if it was not down in the
		// previous frame
		return !it->second;
	}

	bool InputManager::isKeyReleased(int key) {
		// If the key is down it can't be released
		if (isKeyDown(key)) {
			return false;
		}

		// The key is not down, attempt to find it in the previous frame's map
		std::unordered_map<int, bool>::iterator it = m_prevKeys.find(key);
		if (it == m_prevKeys.end()) {
			// The key is not in the previous frame, it has not been released
			return false;
		}

		// Since the key is not down, it has been released if it was down in the
		// previous frame
		return it->second;
	}

	bool InputManager::isButtonDown(int button) {
		// Attempt to find the button in the button map
		std::unordered_map<int, bool>::iterator it = m_buttons.find(button);
		if (it == m_buttons.end()) {
			// The button was not found, it is not down
			return false;
		}
		// Give the state of the button
		return it->second;
	}

	bool InputManager::isButtonPressed(int button) {
		// If the button is not down it can't be pressed
		if (!isButtonDown(button)) {
			return false;
		}

		// The button is down, attempt to find it in the previous frame's map
		std::unordered_map<int, bool>::iterator it = m_prevButtons.find(button);
		if (it == m_prevButtons.end()) {
			// The button is not in the previous frame, it has just been pressed
			return true;
		}

		// Since the button is down, it has been pressed if it was not down in
		// the previous frame
		return !it->second;
	}

	bool InputManager::isButtonReleased(int button) {
		// If the button is down it can't be released
		if (isButtonDown(button)) {
			return false;
		}

		// The button is not down, attempt to find it in the previous frame's
		// map
		std::unordered_map<int, bool>::iterator it = m_prevButtons.find(button);
		if (it == m_prevButtons.end()) {
			// The button is not in the previous frame, it has not been released
			return false;
		}

		// Since the button is not down, it has been released if it was down in
		// the previous frame
		return it->second;
	}

	glm::vec2 InputManager::getCursorPosition(const Camera* camera) const {
		glm::vec2 cursorPosition = m_cursorPosition;
		
		// Transform the cursor position on the window to its position in the
		// camera's world-space
		cursorPosition.x -= (float)MW::WINDOW.getDimensions().x / 2;
		cursorPosition.y -= (float)MW::WINDOW.getDimensions().y / 2;
		cursorPosition /= camera->scale;
		cursorPosition.x += camera->position.x;
		cursorPosition.y += camera->position.y;

		return cursorPosition;
	}

	bool InputManager::isGamepadButtonDown(int gamepad, unsigned int button) {
		// Ensure the button is in bounds
		if (button > GLFW_GAMEPAD_BUTTON_LAST) {
			return false;
		}
		// Attempt to find the gamepad
		std::unordered_map<int, GLFWgamepadstate>::iterator it
			= m_gamepads.find(gamepad);
		if (it == m_gamepads.end()) {
			if (glfwJoystickPresent(gamepad)) {
				if (glfwJoystickIsGamepad(gamepad)) {
					JoystickCallback(gamepad, GLFW_CONNECTED);
					return isGamepadButtonDown(gamepad, button);
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		// The gamepad has been found
		if (m_gamepads[gamepad].buttons[button] == GLFW_PRESS) {
			return true;
		}
		else {
			return false;
		}
	}

	bool InputManager::isGamepadButtonPressed(int gamepad,
		unsigned int button) {
		// Ensure the button is in bounds
		if (button > GLFW_GAMEPAD_BUTTON_LAST) {
			return false;
		}

		if (!isGamepadButtonDown(gamepad, button)) {
			return false;
		}
		// The button is down, find the gamepad in the last frame
		std::unordered_map<int, GLFWgamepadstate>::iterator it
			= m_prevGamepads.find(gamepad);
		if (it == m_prevGamepads.end()) {
			return true;
		}
		if (m_prevGamepads[gamepad].buttons[button] == GLFW_RELEASE) {
			return true;
		}
		else {
			return false;
		}
	}

	bool InputManager::isGamepadButtonReleased(int gamepad,
		unsigned int button) {
		// Ensure the button is in bounds
		if (button > GLFW_GAMEPAD_BUTTON_LAST) {
			return false;
		}

		if (isGamepadButtonDown(gamepad, button)) {
			return false;
		}
		// The button is down, find the gamepad in the last frame
		std::unordered_map<int, GLFWgamepadstate>::iterator it
			= m_prevGamepads.find(gamepad);
		if (it == m_prevGamepads.end()) {
			return false;
		}
		if (m_prevGamepads[gamepad].buttons[button] == GLFW_PRESS) {
			return true;
		}
		else {
			return false;
		}
	}

#define AXIS_VALUE_NOT_FOUND -1.1f

	bool InputManager::isGamepadAxisMoved(int gamepad, unsigned int axis) {
		// Get the current position of the axis
		float cpos = getGamepadAxisPosition(gamepad, axis);
		if (cpos == AXIS_VALUE_NOT_FOUND) {
			return false;
		}
		
		// Attempt to find the gamepad in the previous frame
		std::unordered_map<int, GLFWgamepadstate>::iterator it
			= m_prevGamepads.find(gamepad);
		if (it == m_prevGamepads.end()) {
			return false;
		}

		// The gamepad was found, the axis was moved if its old value is not
		// equal to the current one.
		return cpos != it->second.axes[axis];
	}

	float InputManager::getGamepadAxisPosition(int gamepad, unsigned int axis) {
		// Ensure the axis is in bounds
		if (axis > GLFW_GAMEPAD_AXIS_LAST) {
			return AXIS_VALUE_NOT_FOUND;
		}

		// Attempt to find the gamepad in the map by its ID
		std::unordered_map<int, GLFWgamepadstate>::iterator it
			= m_gamepads.find(gamepad);
		if (it == m_gamepads.end()) {
			return AXIS_VALUE_NOT_FOUND;
		}
		
		return m_gamepads[gamepad].axes[axis];
	}

	unsigned int InputManager::getGamepadCount() const {
		return (unsigned int)m_gamepads.size();
	}
}