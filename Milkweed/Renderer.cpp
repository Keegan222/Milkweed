/*
* File: Renderer.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.01.2211
*/

#include <GL/glew.h>

#include "MW.h"
#include "Renderer.h"

namespace MW {
	void Renderer::init(const glm::vec3& clearColor) {
		// Set the clear color
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		m_vertices = {
			// Triangle 1
			// Position 1
			1.0f, 1.0f,
			// Position 2
			0.0f, 1.0f,
			// Position 3
			0.0f, 0.0f,

			// Triangle 2
			// Position 1
			1.0f, 1.0f,
			// Position 2
			0.0f, 0.0f,
			// Position 3
			1.0f, 0.0f
		};

		// Create and bind the VAO and VBO
		glGenVertexArrays(1, &m_VAOID);
		glBindVertexArray(m_VAOID);
		glGenBuffers(1, &m_VBOID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);

		m_shader.init("Assets/Shaders/vertex_shader.glsl",
			"Assets/Shaders/fragment_shader.glsl",
			{ VertexAttribute("inPosition", 2, GL_FLOAT, GL_FALSE,
				2 * sizeof(float), 0) });
	}

	void Renderer::begin() {
		// Clear the color buffer to the clearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::end() {
		m_shader.begin();

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(),
			&(m_vertices[0]), GL_STATIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_shader.end();
	}

	void Renderer::destroy() {
		// Unbind and delete the VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_VBOID);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_VAOID);
	}
}