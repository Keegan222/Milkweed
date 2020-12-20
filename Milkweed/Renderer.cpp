/*
* File: Renderer.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.01.2211
*/

#include <algorithm>
#include <GL/glew.h>

#include "MW.h"
#include "Renderer.h"
#include "Sprite.h"
#include "ResourceManager.h"

namespace MW {
	void Renderer::init(const glm::vec3& clearColor) {
		// Give the window the OpenGL context
		glfwMakeContextCurrent(App::WINDOW.getWindowHandle());

		// Initialize GLEW and get the running OpenGL version
		if (glewInit() != GLEW_OK) {
			// GLEW could not be initialize
			App::Log("Failed to initialize GLEW");
			return;
		}

		const GLubyte* version = glGetString(GL_VERSION);
		App::Log("OpenGL version: " + std::string((char*)version));

		// Set the clear color
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);

		// Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Create and bind the VAO, VBO, and IBO
		glGenVertexArrays(1, &m_VAOID);
		glBindVertexArray(m_VAOID);
		glGenBuffers(1, &m_VBOID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOID);
		glGenBuffers(1, &m_IBOID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOID);

		// Default to texture slot 0
		// TODO: This should probably be changed later
		glActiveTexture(GL_TEXTURE0);
	}

	void Renderer::begin() {
		// Clear the color buffer to the clearColor and the sprites from last
		// frame
		glClear(GL_COLOR_BUFFER_BIT);
		m_sprites.clear();
	}

	void Renderer::submit(const std::vector<Sprite*>& sprites, Shader* shader) {
		if (sprites.empty()) {
			return;
		}
		std::unordered_map<Shader*, std::vector<Sprite*>>::iterator it
			= m_sprites.find(shader);
		if (it == m_sprites.end()) {
			// The shader was already found in this frame, it must be added
			m_sprites[shader] = std::vector<Sprite*>(0);
		}

		// Add the sprites to their shader
		for (Sprite* sprite : sprites) {
			m_sprites[shader].push_back(sprite);
		}
	}

	bool compareSpriteTexture(const Sprite* a, const Sprite* b) {
		return a->texture->textureID < b->texture->textureID;
	}
	
	bool compareSpriteDepth(const Sprite* a, const Sprite* b) {
		return a->position.z < b->position.z;
	}

	void Renderer::end() {
		for (std::pair<Shader*, std::vector<Sprite*>> shaderBatch : m_sprites) {
			// Get the shader to render this batch of sprites with
			Shader* shader = shaderBatch.first;
			std::vector<Sprite*> sprites = shaderBatch.second;
			// Sort the sprites by their texture ID's
			if (m_sortType == SortType::TEXTURE) {
				std::stable_sort(sprites.begin(), sprites.end(),
					compareSpriteTexture);
			}
			else if (m_sortType == SortType::DEPTH) {
				std::stable_sort(sprites.begin(), sprites.end(),
					compareSpriteDepth);
			}
			// Start the shader
			shader->begin();

			// Initialize the data to send to OpenGL
			std::vector<float> vertexData;
			unsigned int spriteCount = 0;

			// Bind the first texture of the first texture group
			GLuint currentTextureID = sprites[0]->texture->textureID;
			glBindTexture(GL_TEXTURE_2D, currentTextureID);

			for (unsigned int i = 0; i < sprites.size(); i++) {
				Sprite* sprite = sprites[i];
				// Test if a new texture group has started
				if (currentTextureID != sprite->texture->textureID) {
					if (spriteCount > 0) {
						// If there were sprites in the last texture batch,
						// draw them
						drawVertices(vertexData, spriteCount);
					}

					// Reset the vertex data and bind the new texture
					currentTextureID = sprite->texture->textureID;
					glBindTexture(GL_TEXTURE_2D, currentTextureID);
					vertexData.clear();
					spriteCount = 0;
				}

				// Add this sprite's data to the current texture group's vertex
				// data to be uploaded to OpenGL
				for (float f : sprite->getVertexData()) {
					vertexData.push_back(f);
				}
				spriteCount++;
			}

			// Draw the remaining vertex data from the final texture group
			drawVertices(vertexData, spriteCount);

			// Stop this shader
			shader->end();
		}
	}

	void Renderer::drawVertices(const std::vector<float>& vertexData,
		int spriteCount) {
		// Upload the vertex data to OpenGL
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * vertexData.size(),
			&(vertexData[0]), GL_STATIC_DRAW);

		// Generate and upload the indices to render the vertex data with
		std::vector<unsigned int> indices(6 * spriteCount);
		for (unsigned int i = 0; i < spriteCount; i++) {
			for (unsigned int j = 0; j < 6; j++) {
				unsigned int index = j + (6 * i);
				indices[index] = Sprite::VERTEX_INDICES[index % 6] + (4 * i);
			}
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indices.size(),
			&(indices[0]), GL_STATIC_DRAW);

		// Draw the indices with OpenGL
		glDrawElements(GL_TRIANGLES, indices.size(),
			GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::destroy() {
		// Unbind and delete the VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_VBOID);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_VAOID);
	}
}