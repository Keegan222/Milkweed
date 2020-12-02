/*
* File: Renderer.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.01.2209
*/

#pragma once

#include <unordered_map>

#include "Camera.h"
#include "Shader.h"
#include "ResourceManager.h"

namespace MW {
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
		* Process input to the renderer
		* TODO: Remove this later
		*/
		void processInput();
		/*
		* Update the renderer's camera
		* TODO: Remove this later
		*/
		void update(float deltaTime);
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
		Texture* m_texture;
	};
}