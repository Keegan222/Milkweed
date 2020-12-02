/*
* File: Renderer.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.01.2211
*/

#include <GL/glew.h>

#include "MW.h"
#include "Renderer.h"
#include "Sprite.h"
#include "ResourceManager.h"

namespace MW {
	void Renderer::init(const glm::vec3& clearColor) {
		// Set the clear color
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
		m_vertices = {
			// Triangle 1
			// Vertex 1
			// Position
			100.0f, 100.0f,
			// Texture coords
			1.0f, 1.0f,
			// Vertex 2
			// Position
			0.0f, 100.0f,
			// Texture coords
			0.0f, 1.0f,
			// Vertex 3
			// Position
			0.0f, 0.0f,
			// Texture coords
			0.0f, 0.0f,

			// Triangle 2
			// Vertex 1
			// Position
			100.0f, 100.0f,
			// Texture coords
			1.0f, 1.0f,
			// Vertex 2
			// Position
			0.0f, 0.0f,
			// Texture coords
			0.0f, 0.0f,
			// Vertex 3
			// Position
			100.0f, 0.0f,
			// Texture coords
			1.0f, 0.0f,
		};

		// Create and bind the VAO and VBO
		glGenVertexArrays(1, &m_VAOID);
		glBindVertexArray(m_VAOID);
		glGenBuffers(1, &m_VBOID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);

		// Initialize the shader and the camera
		// TODO: Remove this later
		m_shader.init(
			"Assets/Shaders/vertex_shader.glsl",
			"Assets/Shaders/fragment_shader.glsl",
			{
				// Position
				VertexAttribute("inPosition", 2, GL_FLOAT, GL_FALSE,
					4 * sizeof(float), 0),
				// Texture coordinates
				VertexAttribute("inTextureCoords", 2, GL_FLOAT, GL_FALSE,
					4 * sizeof(float), 2 * sizeof(float)),
			}
		);
		m_camera.init(glm::ivec2(800, 600));

		m_texture = App::RESOURCES.getTexture("Assets/Textures/texture.png");
	}

	void Renderer::begin() {
		// Clear the color buffer to the clearColor
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::end() {
		m_shader.begin();

		m_shader.upload4x4Matrix("cameraMatrix", m_camera.getCameraMatrix());
		m_shader.uploadInt("textureSampler", 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture->textureID);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(),
			&(m_vertices[0]), GL_STATIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_shader.end();
	}

	void Renderer::processInput() {
		if (App::INPUT.isKeyDown(GLFW_KEY_A)) {
			m_camera.velocity.x = -5.0f;
		}
		else if (App::INPUT.isKeyDown(GLFW_KEY_D)) {
			m_camera.velocity.x = 5.0f;
		}
		else {
			m_camera.velocity.x = 0.0f;
		}

		if (App::INPUT.isKeyDown(GLFW_KEY_S)) {
			m_camera.velocity.y = -5.0f;
		}
		else if (App::INPUT.isKeyDown(GLFW_KEY_W)) {
			m_camera.velocity.y = 5.0f;
		}
		else {
			m_camera.velocity.y = 0.0f;
		}

		if (App::INPUT.isKeyDown(GLFW_KEY_Q)) {
			m_camera.scaleVelocity = -0.01f;
		}
		else if (App::INPUT.isKeyDown(GLFW_KEY_E)) {
			m_camera.scaleVelocity = 0.01f;
		}
		else {
			m_camera.scaleVelocity = 0.0f;
		}
	}

	void Renderer::update(float deltaTime) {
		m_camera.update(deltaTime);
		if (m_camera.scale < 0.1f) {
			m_camera.scale = 0.1f;
			m_camera.scaleVelocity = 0.0f;
		}
	}

	void Renderer::destroy() {
		// Unbind and delete the VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_VBOID);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_VAOID);
	}
}