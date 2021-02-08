/*
* File: Window.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.10.20.2025
*/

#include "MW.h"
#include "Window.h"

namespace MW {
	Window Window::m_instance;

	bool Window::init(const std::string& title, const glm::ivec2& dimensions,
		bool fullScreen) {
		// Initialize GLFW
		if (glfwInit() != GLFW_TRUE) {
			// GLFW could not be initialized
			App::LOG << "Failed to initialize GLFW\n";
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
			App::LOG << "GLFW failed to create window\n";
			return false;
		}

		setFullScreen(fullScreen);

		// Give the window the OpenGL context
		glfwMakeContextCurrent(App::WINDOW.getWindowHandle());

		App::LOG << "Opened window with dimensions (" << m_dimensions.x << ", "
			<< m_dimensions.y << ")\n";

		// The window was successfully created
		m_initialized = true;
		return true;
	}

	void Window::setDimensions(const glm::ivec2& dimensions) {
		// Window dimensions are fixed if this window is in fullscreen mode
		if (m_fullScreen) {
			return;
		}

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
			// Set the window to its fullscreen dimensions and let it take up
			// the whole monitor
			glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0,
				videoMode->width, videoMode->height, GLFW_DONT_CARE);
			glfwSetWindowSize(m_window, videoMode->width, videoMode->height);
			m_dimensions = glm::ivec2(videoMode->width, videoMode->height);
			glViewport(0, 0, videoMode->width, videoMode->height);
		}
		else {
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