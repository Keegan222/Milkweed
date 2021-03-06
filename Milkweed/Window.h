/*
* File: Window.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.20
*/

#ifndef MW_WINDOW_H
#define MW_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace Milkweed {
	/*
	* A wrapper for a GLFW window handle
	*/
	class Window {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		Window(Window& w) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static Window& getInstance() {
			return m_instance;
		}

		/*
		* Create and open a window using GLFW
		*
		* @param title: The title to appear at the top of this window
		* @param dimensions: The desired dimensions of this window in pixels
		* @param fullScreen: Whether this window should ignore the title and
		*	dimensions and appear in full screen mode
		* @return Whether the window could be successfully opened
		*/
		bool init(const std::string& title, const glm::ivec2& dimensions,
			bool fullScreen);
		/*
		* Get the GLFW handle for this window
		*/
		GLFWwindow* getWindowHandle() { return m_window; }
		/*
		* Get the title of this window
		*/
		std::string getTitle() const { return m_title; }
		/*
		* Get the dimensions of this window in pixels
		*/
		glm::ivec2 getDimensions() const { return m_dimensions; }
		/*
		* Set the dimensions of this window in pixels
		*/
		void setDimensions(const glm::ivec2& dimensions);
		/*
		* Test whether this window is in fullscreen mode
		*/
		bool isFullScreen() const { return m_fullScreen; }
		/*
		* Set whether this window should appear in fullscreen mode
		*/
		void setFullScreen(bool fullScreen);

	private:
		// The singleton instance of this class
		static Window m_instance;
		/*
		* The constructor is disabled for this class
		*/
		Window() {}

		// The GLFW handle for this window
		GLFWwindow* m_window = nullptr;
		// The dimensions of this window in pixels
		glm::ivec2 m_dimensions = glm::ivec2();
		// The dimensions of this window when in windowed mode
		glm::ivec2 m_windowedDimensions = glm::ivec2();
		// The title of this window
		std::string m_title = "";
		// Whether this window is in fullscreen mode
		bool m_fullScreen = false;
		// Whether this window has been initialized or not
		bool m_initialized = false;

		/*
		* The size of the window has changes, notify all the application's
		* scenes
		*/
		void updateSize();
	};
}

#endif