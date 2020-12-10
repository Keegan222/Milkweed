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
		* Submit a group of sprites to be rendered this frame with the shader to
		* render them with
		* 
		* @param sprites: A set of pointers to sprites to be rendered
		* @param shader: A pointer to the shader to render these sprites with
		*/
		void submit(const std::vector<Sprite*>& sprites, Shader* shader);
		/*
		* End a frame and draw it on the screen
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
		// The index buffer for this renderer
		GLuint m_IBOID = 0;
		// The sprites to be rendered this frame
		std::unordered_map<Shader*, std::vector<Sprite*>> m_sprites;

		/*
		* Draw a set of sprites with a single texture
		*/
		void drawVertices(const std::vector<float>& vertexData,
			unsigned int spriteCount);
	};
}