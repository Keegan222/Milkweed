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
	* The ways to sort sprites to be rendered
	*/
	enum class SortType {
		TEXTURE, DEPTH,
	};

	/*
	* The Milkweed engine's utility for drawing graphics
	*/
	class Renderer {
	public:
		/*
		* Disable copy constructor
		*/
		Renderer(Renderer& r) = delete;
		/*
		* Set up this renderer in OpenGL
		*/
		void init();
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
		* Submit a string of text to be converted to sprites and rendered this
		* frame in the given color
		* 
		* @param text: The text label to draw
		* @param font: The font the draw the text int
		* @param shader: The shader to render the text with
		*/
		void submit(const std::string& text, const glm::vec3& position,
			const glm::vec4& bounds, float scale, const glm::vec3& color,
			Font* font, Shader* shader);
		/*
		* End a frame and draw it on the screen
		*/
		void end();
		/*
		* Set the type of sorting to perform when rendering sprites
		*/
		void setSortType(SortType sortType) { m_sortType = sortType; }
		/*
		* Free this renderer's memory and stop using it
		*/
		void destroy();
		/*
		* Get the current clear color
		*/
		glm::vec3 getClearColor() const { return m_clearColor; }
		/*
		* Set the RGB color to clear the screen to each frame
		* 
		* @param clearColor: Normalized RGB color
		*/
		void setClearColor(const glm::vec3& clearColor) {
			m_clearColor = clearColor;
			glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		}
		/*
		* Get the singleton instance of this class
		*/
		static Renderer& getInstance() {
			return m_instance;
		}
		// TODO: Remove this function later it's for debugging
		void dumpNextFrame() { m_dumpFrame = true; }

	private:
		// TODO: Variable for debugging, remove later
		bool m_dumpFrame = false;
		// The vertex array for this renderer
		GLuint m_VAOID = 0;
		// The vertex data buffer for this renderer
		GLuint m_VBOID = 0;
		// The index buffer for this renderer
		GLuint m_IBOID = 0;
		// The sprites to be rendered this frame
		std::unordered_map<Shader*, std::vector<Sprite*>> m_sprites;
		// The text characters to render this frame
		std::unordered_map<Shader*, std::vector<Sprite>> m_text;
		// The type of sorting to perform when rendering sprites
		SortType m_sortType = SortType::DEPTH;
		// Normalized RGB color to clear the screen to
		glm::vec3 m_clearColor = glm::vec3();
		// Singleton instance of this class
		static Renderer m_instance;

		// Disable constructor
		Renderer() {}
		/*
		* Draw a set of sprites with a single texture
		*/
		void drawVertices(const std::vector<float>& vertexData,
			const std::vector<unsigned int>& indices);
	};
}