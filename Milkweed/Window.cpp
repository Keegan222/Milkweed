/*
* File: Window.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.2025
*/

#include "MW.h"
#include "Window.h"

namespace MW {
	bool Window::init(const WindowAttributes& attrib) {
		// Initialize GLFW
		if (glfwInit() != GLFW_TRUE) {
			// GLFW could not be initialized
			App::LOG << "Failed to initialize GLFW\n";
			return false;
		}

		// Get the video mode (description of the monitor displaying this
		// application)
		const GLFWvidmode* videoMode
			= glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		if (!attrib.fullScreen) {
			// Create the window in windowed mode
			m_window = glfwCreateWindow(attrib.dimensions.x,
				attrib.dimensions.y, attrib.title.c_str(), NULL, NULL);
			if (m_window == NULL) {
				// The window could not be created
				App::LOG << "GLFW failed to create window\n";
				return false;
			}
			// Center the window in the screen
			glfwSetWindowPos(m_window,
				(videoMode->width - attrib.dimensions.x) / 2,
				(videoMode->height - attrib.dimensions.y) / 2);

			// Set the dimensions of the window
			m_dimensions = attrib.dimensions;
		}
		else {
			// Create the window in fullscreen mode
			m_window = glfwCreateWindow(videoMode->width, videoMode->height,
				attrib.title.c_str(), glfwGetPrimaryMonitor(), NULL);
			if (m_window == NULL) {
				// The window could not be created
				App::LOG << "GLFW failed to create fullscreen window\n";
				return false;
			}

			// Set the dimensions of the window
			m_dimensions = glm::ivec2(videoMode->width, videoMode->height);
		}

		App::LOG << "Opened window with dimensions (" << m_dimensions.x << ", "
			<< m_dimensions.y << ")\n";

		// The window was successfully created
		return true;
	}
}