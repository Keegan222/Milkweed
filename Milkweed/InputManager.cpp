/*
* File: InputManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.22.1703
*/

#include "MW.h"
#include "InputManager.h"

namespace MW {
	InputManager InputManager::m_instance;

	void InputManager::init() {
		// Add the GLFW callback functions
		glfwSetKeyCallback(App::WINDOW.getWindowHandle(), key_callback);
		glfwSetCharCallback(App::WINDOW.getWindowHandle(), char_callback);
		glfwSetMouseButtonCallback(App::WINDOW.getWindowHandle(),
			button_callback);
		glfwSetCursorPosCallback(App::WINDOW.getWindowHandle(),
			cursor_callback);
	}

	void InputManager::key_callback(GLFWwindow* window, int key, int scancode,
		int action, int mods) {
		// Decide whether the key was pressed or released and modify the key
		// in the application's input manager
		switch (action) {
		case GLFW_PRESS:
			// Update the state of the key and notify input listeners
			App::INPUT.m_keys[key] = true;
			for (InputListener* l : App::INPUT.m_listeners) {
				l->keyPressed(key);
			}
			break;
		case GLFW_RELEASE:
			// Update the state of the key and notify input listeners
			App::INPUT.m_keys[key] = false;
			for (InputListener* l : App::INPUT.m_listeners) {
				l->keyReleased(key);
			}
			break;
		}
	}

	void InputManager::char_callback(GLFWwindow* window,
		unsigned int codepoint) {
		// Notify input listeners
		for (InputListener* l : App::INPUT.m_listeners) {
			l->textTyped((char)codepoint);
		}
	}
	
	void InputManager::button_callback(GLFWwindow* window, int button,
		int action, int mods) {
		// Decide whether the button was pressed or released and modify it in
		// the application's input manager
		switch (action) {
		case GLFW_PRESS:
			// Update the state of the button and notify input listeners
			App::INPUT.m_buttons[button] = true;
			for (InputListener* l : App::INPUT.m_listeners) {
				l->buttonPressed(button);
			}
			break;
		case GLFW_RELEASE:
			// Update the state of the button and notify input listeners
			App::INPUT.m_buttons[button] = false;
			for (InputListener* l : App::INPUT.m_listeners) {
				l->buttonReleased(button);
			}
			break;
		}
	}

	void InputManager::cursor_callback(GLFWwindow* window, double x, double y) {
		// Set the new cursor position and notify input listeners (invert the
		// y-axis)
		for (InputListener* l : App::INPUT.m_listeners) {
			l->cursorMoved();
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
		for (std::pair<int, bool> k : m_keys) {
			m_prevKeys[k.first] = k.second;
		}
		// Push the button states into the next frame
		for (std::pair<int, bool> b : m_buttons) {
			m_prevButtons[b.first] = b.second;
		}

		// Check GLFW for new events
		glfwPollEvents();

		// Update the cursor position
		double x = 0.0, y = 0.0;
		glfwGetCursorPos(App::WINDOW.getWindowHandle(), &x, &y);
		m_cursorPosition.x = (float)x;
		m_cursorPosition.y = App::WINDOW.getDimensions().y - (float)y;
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
		cursorPosition.x -= (float)App::WINDOW.getDimensions().x / 2;
		cursorPosition.y -= (float)App::WINDOW.getDimensions().y / 2;
		cursorPosition /= camera->scale;
		cursorPosition.x += camera->position.x;
		cursorPosition.y += camera->position.y;

		return cursorPosition;
	}
}