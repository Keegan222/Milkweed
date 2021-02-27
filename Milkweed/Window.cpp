/*
* File: Window.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.20
*/

#include "MW.h"

namespace Milkweed {
	Window Window::m_instance;

	bool Window::init(const std::string& title, const glm::ivec2& dimensions,
		bool fullScreen) {
		MWLOG(Info, Window, "Initializing GLFW and opening window");

		// Initialize GLFW
		if (glfwInit() != GLFW_TRUE) {
			// GLFW could not be initialized
			MWLOG(Error, Window, "Could not initialize GLFW");
			return false;
		}

		m_title = title;
		m_dimensions = dimensions;
		m_windowedDimensions = dimensions;
		m_fullScreen = fullScreen;

		// Get the video mode (description of the monitor displaying this
		// application)
		const GLFWvidmode* videoMode
			= glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Create the window in windowed mode
		m_window = glfwCreateWindow(dimensions.x,
			dimensions.y, title.c_str(), NULL, NULL);
		if (m_window == NULL) {
			// The window could not be created
			MWLOG(Error, Window, "Failed to create GLFW window");
			return false;
		}

		setFullScreen(fullScreen);

		// Give the window the OpenGL context
		glfwMakeContextCurrent(MW::WINDOW.getWindowHandle());

		// The window was successfully created
		m_initialized = true;
		return true;
	}

	void Window::setDimensions(const glm::ivec2& dimensions) {
		// Window dimensions are fixed if this window is in fullscreen mode
		if (m_fullScreen) {
			MWLOG(Info, Window, "Setting window dimensions for windowed mode (",
				dimensions.x, ", ", dimensions.y, "), no change now");
			m_windowedDimensions = dimensions;
			return;
		}

		MWLOG(Info, Window, "Updating window dimensions to (", dimensions.x,
			", ", dimensions.y, ")");
		// Set the current dimensions, update the windowed dimensions, and
		// update the window
		m_dimensions = dimensions;
		m_windowedDimensions = dimensions;
		glfwSetWindowSize(m_window, dimensions.x, dimensions.y);
		glViewport(0, 0, dimensions.x, dimensions.y);
	}

	void Window::setFullScreen(bool fullScreen) {
		// Do not change if the fullscreen mode doesn't need to change
		if (m_fullScreen == fullScreen && m_initialized) {
			return;
		}

		const GLFWvidmode* videoMode
			= glfwGetVideoMode(glfwGetPrimaryMonitor());

		if (fullScreen) {
			MWLOG(Info, Window, "Setting window to fullscreen mode");
			// Set the window to its fullscreen dimensions and let it take up
			// the whole monitor
			glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0,
				videoMode->width, videoMode->height, GLFW_DONT_CARE);
			glfwSetWindowSize(m_window, videoMode->width, videoMode->height);
			m_dimensions = glm::ivec2(videoMode->width, videoMode->height);
			glViewport(0, 0, videoMode->width, videoMode->height);
		}
		else {
			MWLOG(Info, Window, "Setting window to windowed mode, dimensions (",
				m_dimensions.x, ", ", m_dimensions.y, ")");
			// Set the window to its windowed dimensions and reset its position
			// to the center of the monitor
			glfwSetWindowMonitor(m_window, nullptr, 0, 0,
				m_windowedDimensions.x, m_windowedDimensions.y,
				GLFW_DONT_CARE);
			glfwSetWindowSize(m_window, m_windowedDimensions.x,
				m_windowedDimensions.y);
			glfwSetWindowPos(m_window,
				(videoMode->width - m_windowedDimensions.x) / 2,
				(videoMode->height - m_windowedDimensions.y) / 2);
			m_dimensions = m_windowedDimensions;
			glViewport(0, 0, m_dimensions.x, m_dimensions.y);
		}

		m_fullScreen = fullScreen;
	}
}