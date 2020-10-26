/*
* File: InputManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.22.1656
*/

#pragma once

#include <unordered_map>

namespace MW {
	/*
	* An interface for listening for GLFW user input callbacks in the
	* application's input manager
	*/
	class InputListener {
	public:
		/*
		* A key has been pressed on the keyboard
		*/
		virtual void keyPressed(int key) = 0;
		/*
		* A key has been released on the keyboard
		*/
		virtual void keyReleased(int key) = 0;
		/*
		* Text has been typed on the keyboard
		*/
		virtual void textTyped(char text) = 0;
		/*
		* A button has been pressed on the mouse
		*/
		virtual void buttonPressed(int button) = 0;
		/*
		* A button has been released on the mouse
		*/
		virtual void buttonReleased(int button) = 0;
		/*
		* The mouse cursor has been moved on the window
		*/
		virtual void cursorMoved() = 0;
	};

	/*
	* Milkweed engine application's utility for handling user mouse and keyboard
	* input from GLFW
	*/
	class InputManager {
	public:
		/*
		* Register this input manager's callback functions with GLFW
		*/
		void init();
		/*
		* Add an input listener to this input manager
		* 
		* @param listener: The input listener to add
		*/
		void addInputListener(InputListener* listener);
		/*
		* Remove an input listener from this input manager
		* 
		* @param listener: The input listener to remove
		*/
		void removeInputListener(InputListener* listener);
		/*
		* Move keyboard and mouse button states to the next frame and poll GLFW
		* for events
		*/
		void update();
		/*
		* Test whether a key is currently down
		* 
		* @param key: The key to test
		*/
		bool isKeyDown(int key);
		/*
		* Test whether a key has just been pressed
		* 
		* @param key: The key to test
		*/
		bool isKeyPressed(int key);
		/*
		* Test whether a key has just been released
		* 
		* @param key: The key to test
		*/
		bool isKeyReleased(int key);
		/*
		* Test whether a mouse button is currently down
		* 
		* @param button: The button to test
		*/
		bool isButtonDown(int button);
		/*
		* Test whether a button has just been pressed
		*
		* @param key: The button to test
		*/
		bool isButtonPressed(int button);
		/*
		* Test whether a button has just been released
		*
		* @param key: The button to test
		*/
		bool isButtonReleased(int button);

	private:
		// The input listeners attached to this input manager
		std::list<InputListener*> m_listeners;
		// The states of the keys on the keyboard
		std::unordered_map<int, bool> m_keys;
		// The states of the keys on the keyboard in the last frame
		std::unordered_map<int, bool> m_prevKeys;
		// The states of the buttons on the mouse
		std::unordered_map<int, bool> m_buttons;
		// The states of the button on the mouse in the last frame
		std::unordered_map<int, bool> m_prevButtons;
		// The position of the mouse cursor on the window
		glm::vec2 m_cursorPosition = glm::vec2();

		/*
		* GLFW callback function for keyboard input events
		*/
		static void key_callback(GLFWwindow* window, int key, int scancode,
			int action, int mods);
		/*
		* GLFW callback function for text input events
		*/
		static void char_callback(GLFWwindow* window, unsigned int codepoint);
		/*
		* GLFW callback function for mouse button input events
		*/
		static void button_callback(GLFWwindow* window, int button, int action,
			int mods);
		/*
		* GLFW callback function for mouse cursor input events
		*/
		static void cursor_callback(GLFWwindow* window, double x, double y);
	};
}