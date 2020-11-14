/*
* File: Renderer.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.01.2209
*/

#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include "Shader.h"

namespace MW {
	/*
	* A 2D camera used to position sprites on the screen
	*/
	class Camera {
	public:
		/*
		* Initialize this camera with the window's dimensions
		* 
		* @param windowDimensions: The dimensions of the window in pixels
		*/
		void init(const glm::ivec2& windowDimensions);
		/*
		* Get the camera's projection matrix to pass to a shader
		*/
		glm::mat4 getCameraMatrix() const { return m_orthoMatrix; }

	private:
		// The camera's blank projection matrix
		glm::mat4 m_orthoMatrix;
	};

	/*
	* The Milkweed engine's utility for drawing graphics
	*/
	class Renderer {
	public:
		/*
		* Set up this renderer in OpenGL
		* 
		* @param clearColor: The background color of the window in normalized
		* float RGB values (0.0f - 1.0f)
		*/
		void init(const glm::vec3& clearColor);
		/*
		* Begin a frame
		*/
		void begin();
		/*
		* End a frame and draw it
		*/
		void end();
		/*
		* Free this renderer's memory and stop using it
		*/
		void destroy();

	private:
		// The vertex array for this renderer
		GLuint m_VAOID = 0;
		// The vertex data buffer for this renderer
		GLuint m_VBOID = 0;
		
		std::vector<float> m_vertices;
		Shader m_shader;
		Camera m_camera;
	};
}