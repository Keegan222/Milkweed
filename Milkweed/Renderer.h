/*
* File: Renderer.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.01
*/

#ifndef MW_RENDERER_H
#define MW_RENDERER_H

#include <unordered_map>

#include "Camera.h"
#include "Resources.h"
#include "Shader.h"

namespace Milkweed {
	/*
	* The justification to use when position text for rendering
	*/
	enum class Justification {
		LEFT, CENTER, RIGHT, TOP, BOTTOM
	};

	/*
	* The Milkweed framework's utility for drawing graphics
	*/
	class Renderer {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		Renderer(Renderer& r) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static Renderer& getInstance() {
			return m_instance;
		}

		/*
		* Set up this renderer in OpenGL
		* 
		* @return Whether the renderer could be successfully initialized
		*/
		bool init();
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
		* @param text: The text to draw this frame
		* @param position: The position to draw this text label at
		* @param bounds: The width and height of the rectangle to draw the text
		* in
		* @param scale: The factor to scale the size of the characters in font
		* by when drawing text
		* @param font: The typeface to draw this text in
		* @param shader: The text shader to use to draw this text
		* @param hJustification: The justification of the text within its bounds
		* on the x-axis
		* @param vJustification: The justification of the text within its bounds
		* on the y-axis
		*/
		void submit(const std::string& text, const glm::vec3& position,
			const glm::vec2& bounds, float scale, Font* font, Shader* shader,
			Justification hJustification, Justification vJustification);
		/*
		* End a frame and draw it on the screen
		*/
		void end();
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
		void setClearColor(const glm::vec3& clearColor);
		/*
		* Prepare the renderer to dump the next frame's rendering information
		* to the application's log
		*/
		void dumpNextFrame() { m_dumpFrame = true; }

	private:
		// Singleton instance of this class
		static Renderer m_instance;
		/*
		* The constructor is disabled for this class
		*/
		Renderer() {}

		// Whether to dump the next frame's rendering information to the log
		bool m_dumpFrame = true;
		// The vertex array for this renderer
		GLuint m_VAOID = 0;
		// The vertex data buffer for this renderer
		GLuint m_VBOID = 0;
		// The index buffer for this renderer
		GLuint m_IBOID = 0;
		// The sprites to be rendered this frame
		std::vector<Sprite*> m_sprites;
		// The text characters to render this frame
		std::unordered_map<Shader*, std::vector<Sprite>> m_text;
		// Normalized RGB color to clear the screen to
		glm::vec3 m_clearColor = glm::vec3();

		/*
		* Draw a set of sprites with a single texture
		*/
		void drawVertices(const std::vector<float>& vertexData,
			const std::vector<unsigned int>& indices);
	};
}

#endif