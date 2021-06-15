/*
* File: InputManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.22
*/

#ifndef MW_INPUT_H
#define MW_INPUT_H

#include <unordered_map>

#include "Camera.h"

namespace Milkweed {
	/*
	* Wrapper for GLFW key bindings
	*/
	enum Key : int {
		// Key not recognized
		UNKNOWN = GLFW_KEY_UNKNOWN,

		// Function keys
		F_ESCAPE = GLFW_KEY_ESCAPE,
		F_1 = GLFW_KEY_F1,
		F_2 = GLFW_KEY_F2,
		F_3 = GLFW_KEY_F3,
		F_4 = GLFW_KEY_F4,
		F_5 = GLFW_KEY_F5,
		F_6 = GLFW_KEY_F6,
		F_7 = GLFW_KEY_F7,
		F_8 = GLFW_KEY_F8,
		F_9 = GLFW_KEY_F9,
		F_10 = GLFW_KEY_F10,
		F_11 = GLFW_KEY_F11,
		F_12 = GLFW_KEY_F12,
		F_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
		F_INSERT = GLFW_KEY_INSERT,
		F_DELETE = GLFW_KEY_DELETE,
		F_BACKSPACE = GLFW_KEY_BACKSPACE,
		F_TAB = GLFW_KEY_TAB,
		F_CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
		F_ENTER = GLFW_KEY_ENTER,
		F_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
		F_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
		F_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
		F_LEFT_ALT = GLFW_KEY_LEFT_ALT,
		F_RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
		F_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
		F_UP = GLFW_KEY_UP,
		F_DOWN = GLFW_KEY_DOWN,
		F_LEFT = GLFW_KEY_LEFT,
		F_RIGHT = GLFW_KEY_RIGHT,

		// Symbols
		S_SPACE = GLFW_KEY_SPACE, /*   */
		S_GRAVE_SYMBOL = GLFW_KEY_GRAVE_ACCENT, /* ` */
		S_MINUS = GLFW_KEY_MINUS, /* - */
		S_EQUALS = GLFW_KEY_EQUAL, /* = */
		S_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET, /* [ */
		S_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET, /* ] */
		S_BACKSLASH = GLFW_KEY_BACKSLASH, /* \ */
		S_SEMICOLON = GLFW_KEY_SEMICOLON, /* ; */
		S_APOSTROPHE = GLFW_KEY_APOSTROPHE, /* ' */
		S_COMMA = GLFW_KEY_COMMA, /* , */
		S_PERIOD = GLFW_KEY_PERIOD, /* . */
		S_SLASH = GLFW_KEY_SLASH, /* / */

		// Number keys
		K_0 = GLFW_KEY_0, /* 0 */
		K_1 = GLFW_KEY_1, /* 1 */
		K_2 = GLFW_KEY_2, /* 2 */
		K_3 = GLFW_KEY_3, /* 3 */
		K_4 = GLFW_KEY_4, /* 4 */
		K_5 = GLFW_KEY_5, /* 5 */
		K_6 = GLFW_KEY_6, /* 6 */
		K_7 = GLFW_KEY_7, /* 7 */
		K_8 = GLFW_KEY_8, /* 8 */
		K_9 = GLFW_KEY_9, /* 9 */

		// Letter keys
		K_A = GLFW_KEY_A, /* A */
		K_B = GLFW_KEY_B, /* B */
		K_C = GLFW_KEY_C, /* C */
		K_D = GLFW_KEY_D, /* D */
		K_E = GLFW_KEY_E, /* E */
		K_F = GLFW_KEY_F, /* F */
		K_G = GLFW_KEY_G, /* G */
		K_H = GLFW_KEY_H, /* H */
		K_I = GLFW_KEY_I, /* I */
		K_J = GLFW_KEY_J, /* J */
		K_K = GLFW_KEY_K, /* K */
		K_L = GLFW_KEY_L, /* L */
		K_M = GLFW_KEY_M, /* M */
		K_N = GLFW_KEY_N, /* N */
		K_O = GLFW_KEY_O, /* O */
		K_P = GLFW_KEY_P, /* P */
		K_Q = GLFW_KEY_Q, /* Q */
		K_R = GLFW_KEY_R, /* R */
		K_S = GLFW_KEY_S, /* S */
		K_T = GLFW_KEY_T, /* T */
		K_U = GLFW_KEY_U, /* U */
		K_V = GLFW_KEY_V, /* V */
		K_W = GLFW_KEY_W, /* W */
		K_X = GLFW_KEY_X, /* X */
		K_Y = GLFW_KEY_Y, /* Y */
		K_Z = GLFW_KEY_Z, /* Z */
	};

	/*
	* Wrapper for GLFW mouse button bindings
	*/
	enum Button : int {
		B_1 = GLFW_MOUSE_BUTTON_1,
		B_2 = GLFW_MOUSE_BUTTON_2,
		B_3 = GLFW_MOUSE_BUTTON_3,
		B_4 = GLFW_MOUSE_BUTTON_4,
		B_5 = GLFW_MOUSE_BUTTON_5,
		B_6 = GLFW_MOUSE_BUTTON_6,
		B_7 = GLFW_MOUSE_BUTTON_7,
		B_8 = GLFW_MOUSE_BUTTON_8,
		B_LEFT = GLFW_MOUSE_BUTTON_LEFT,
		B_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
		B_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
	};

	/*
	* Wrapper for default GLFW gamepad button mappings
	*/
	enum GamepadButton : unsigned int {
		G_A = GLFW_GAMEPAD_BUTTON_A,
		G_B = GLFW_GAMEPAD_BUTTON_B,
		G_X = GLFW_GAMEPAD_BUTTON_X,
		G_Y = GLFW_GAMEPAD_BUTTON_Y,
		G_CROSS = GLFW_GAMEPAD_BUTTON_CROSS,
		G_CIRCLE = GLFW_GAMEPAD_BUTTON_CIRCLE,
		G_SQUARE = GLFW_GAMEPAD_BUTTON_SQUARE,
		G_TRIANGLE = GLFW_GAMEPAD_BUTTON_TRIANGLE,
		G_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
		G_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		G_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
		G_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		G_BACK = GLFW_GAMEPAD_BUTTON_BACK,
		G_START = GLFW_GAMEPAD_BUTTON_START,
		G_GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE,
		G_LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		G_RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		G_LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		G_RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
	};

	/*
	* Wrapper for default GLFW gamepad axis mappings
	*/
	enum GamepadAxis : unsigned int {
		GA_LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X,
		GA_LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
		GA_RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
		GA_RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
		GA_LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		GA_RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	};

	/*
	* An interface for listening for GLFW user input callbacks in the
	* application's input manager
	*/
	class InputListener {
	public:
		/*
		* A key has been pressed on the keyboard
		* 
		* @param key: The ID of the relevant key
		*/
		virtual void keyPressed(int key) {};
		/*
		* A key has been released on the keyboard
		* 
		* @param key: The ID of the relevant key
		*/
		virtual void keyReleased(int key) {};
		/*
		* Text has been typed on the keyboard
		* 
		* @param key: The ID of the relevant key
		*/
		virtual void textTyped(char text) {};
		/*
		* A button has been pressed on the mouse
		* 
		* @param key: The ID of the relevant button
		*/
		virtual void buttonPressed(int button) {};
		/*
		* A button has been released on the mouse
		* 
		* @param key: The ID of the relevant button
		*/
		virtual void buttonReleased(int button) {};
		/*
		* The mouse cursor has been moved on the window
		*/
		virtual void cursorMoved() {};
		/*
		* The mouse scroll wheel has been moved on the window
		* 
		* @param distance: The distance in the x and y dimensions the scroll
		* wheel moved.
		*/
		virtual void scrolled(const glm::vec2& distance) {};
		/*
		* A gamepad has been connected
		* 
		* @param gp: The ID of the newly connected gamepad.
		*/
		virtual void gamepadConnected(int gp) {};
		/*
		* A gamepad has been disconnected.
		* 
		* @param gp: The ID of the disconnected gamepad.
		*/
		virtual void gamepadDisconnected(int gp) {};
		/*
		* A button has been pressed on a gamepad
		* 
		* @param gp: The ID of the gamepad on which a button has been pressed.
		* @param button: The ID of the button which has been pressed.
		*/
		virtual void gamepadButtonPressed(int gp, unsigned int button) {};
		/*
		* A button has been released on a gamepad.
		* 
		* @param gp: The ID of the gamepad on which a button has been released.
		* @param button: The ID of the button which has been released.
		*/
		virtual void gamepadButtonReleased(int gp, unsigned int button) {};
		/*
		* An axis has been moved on a gamepad.
		* 
		* @param gp: The ID of the gamepad on which an axis has been moved.
		* @param axis: The ID of the axis which has been moved.
		*/
		virtual void gamepadAxisMoved(int gp, unsigned int axis) {};
	};

	/*
	* Milkweed application's utility for handling user mouse and keyboard
	* input from GLFW
	*/
	class InputManager {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		InputManager(InputManager& im) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static InputManager& getInstance() {
			return m_instance;
		}

		/*
		* Register this input manager's callback functions with GLFW
		*/
		void init();
		/*
		* Add an input listener to this input manager
		* 
		* @param listener: A pointer to the input listener to add
		*/
		void addInputListener(InputListener* listener);
		/*
		* Remove an input listener from this input manager
		* 
		* @param listener: A pointer to the input listener to remove
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
		/*
		* Get the position of the mouse cursor in the window
		*/
		glm::vec2 getCursorPosition() const { return m_cursorPosition; }
		/*
		* Get the position of the mouse cursor in world-space in a given camera
		*/
		glm::vec2 getCursorPosition(const Camera* camera) const;
		/*
		* Test whether a gamepad ID is currently valid
		*/
		bool isGamepadConnected(int gamepad);
		/*
		* Test whether a gamepad button is currently down on any connected
		* gamepad.
		* 
		* @param button: The button to test for.
		* @param gamepad: A pointer to populate with the ID of the first gamepad
		* found to have this button down.
		*/
		bool isGamepadButtonDown(unsigned int button,
			int* gamepad = nullptr);
		/*
		* Test whether a gamepad button is currently down
		*/
		bool isGamepadButtonDown(int gamepad, unsigned int button);
		/*
		* Test whether a gamepad button has just been pressed on any connected
		* gamepad.
		* 
		* @param button: The button to test for.
		* @param gamepad: A pointer to populate with the ID of the first gamepad
		* found to have this button pressed.
		*/
		bool isGamepadButtonPressed(unsigned int button,
			int* gamepad = nullptr);
		/*
		* Test whether a gamepad button has just been pressed
		*/
		bool isGamepadButtonPressed(int gamepad, unsigned int button);
		/*
		* Test whether a gamepad button has just been released on any connected
		* gamepad.
		* 
		* @param button: The button to test for.
		* @param gamepad: A pointer to populate with the ID of the first gamepad
		* found to have this button released.
		*/
		bool isGamepadButtonReleased(unsigned int button,
			int* gamepad = nullptr);
		/*
		* Test whether a gamepad button has just been released
		*/
		bool isGamepadButtonReleased(int gamepad, unsigned int button);
		/*
		* Test whether a gamepad axis has just been moved on any connected
		* gamepad.
		*/
		bool isGamepadAxisMoved(unsigned int axis, float* distance = nullptr,
			int* gamepad = nullptr);
		/*
		* Test whether a gamepad axis has just been moved
		* 
		* @param gamepad: The ID of the gamepad to test
		* @param axis: The axis to test for movement
		* @param distance: A pointer to populate with the distance the axis has
		* moved.
		*/
		bool isGamepadAxisMoved(int gamepad, unsigned int axis,
			float* distance = nullptr);
		/*
		* Get the position of an axis on a gamepad
		*/
		float getGamepadAxisPosition(int gamepad, unsigned int axis);
		/*
		* Get the number of gamepads currently connected
		*/
		unsigned int getGamepadCount() const;

	private:
		// The singleton instance of this class
		static InputManager m_instance;
		/*
		* The constructor is disabled for this class
		*/
		InputManager() {}

		/*
		* GLFW callback function for keyboard input events
		*/
		static void KeyCallback(GLFWwindow* window, int key, int scancode,
			int action, int mods);
		/*
		* GLFW callback function for text input events
		*/
		static void CharCallback(GLFWwindow* window, unsigned int codepoint);
		/*
		* GLFW callback function for mouse button input events
		*/
		static void ButtonCallback(GLFWwindow* window, int button, int action,
			int mods);
		/*
		* GLFW callback function for mouse cursor input events
		*/
		static void CursorCallback(GLFWwindow* window, double x, double y);
		/*
		* GLFW callback function for mouse scroll wheel events
		*/
		static void ScrollCallback(GLFWwindow* window, double xOffset,
			double yOffset);
		/*
		* GLFW callback function for joystick / game controller events
		*/
		static void JoystickCallback(int jid, int event);

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
		// The states of all connected gamepads
		std::unordered_map<int, GLFWgamepadstate> m_gamepads;
		// The states of all connected gamepads in the last frame
		std::unordered_map<int, GLFWgamepadstate> m_prevGamepads;

		// Process input to gamepads for notifying input listeners of events.
		void processGamepadInput();
	};
}

#endif