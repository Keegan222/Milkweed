/*
* File: Window.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.2024
*/

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace MW {
	/*
	* A set of attributes to create a window with
	*/
	struct WindowAttributes {
		// The dimensions of the window in pixels
		glm::ivec2 dimensions;
		// The title of the window
		std::string title;
		// Whether the window should appear in fullscreen mode
		bool fullScreen;

		/*
		* Construct a set of window attributes
		* 
		* @param Dimensions: The desired dimensions of the window in pixels
		* @param Title: The desired title of the window
		* @param FullScreen: Whether the window should appear in fullscreen mode
		*/
		WindowAttributes(const glm::ivec2& Dimensions, const std::string& Title,
			bool FullScreen = false) : dimensions(Dimensions), title(Title),
			fullScreen(FullScreen) {}
	};

	/*
	* A wrapper for a GLFW window handle
	*/
	class Window {
	public:
		/*
		* Create and open a window using GLFW
		* 
		* @param attrib: The set of window attributes to create the window by
		* @return Whether the window could be successfully opened
		*/
		bool init(const WindowAttributes& attrib);
		/*
		* Get the GLFW handle for this window
		*/
		GLFWwindow* getWindowHandle() { return m_window; }
		/*
		* Get the dimensions of this window in pixels
		*/
		glm::ivec2 getDimensions() const { return m_dimensions; }

	private:
		// The GLFW handle for this window
		GLFWwindow* m_window = NULL;
		// The dimensions of this window in pixels
		glm::ivec2 m_dimensions = glm::ivec2();
	};
}