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
	void Renderer::init() {
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
		glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);

		// Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Disable byte-alignment restriction for text rendering
		glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);

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
		m_text.clear();
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

	void Renderer::submit(const Label& label, Font* font,
		Shader* shader) {
		// Get the characters for this string in order
		std::vector<Character> characters;
		for (std::string::const_iterator c = label.text.begin();
			c < label.text.end(); c++) {
			characters.push_back(font->characters[*c]);
		}

		float x = label.position.x;
		for (unsigned int i = 0; i < characters.size(); i++) {
			// Set the position and dimensions of the character and advance
			// to the position of the next one
			characters[i].position.x = x + characters[i].bearing.x * label.scale;
			characters[i].position.y = label.position.y
				- (characters[i].dimensions.y - characters[i].bearing.y)
				* label.scale;
			characters[i].position.z = label.position.z;
			characters[i].dimensions *= label.scale;
			x += (characters[i].offset >> 6) * label.scale;
			
			// Attempt to find the shader in the text map
			std::unordered_map<Shader*, std::vector<Character>>::iterator it
				= m_text.find(shader);
			if (it == m_text.end()) {
				m_text[shader] = std::vector<Character>();
			}
			m_text[shader].push_back(characters[i]);
		}
	}

	bool compareSpriteTexture(const Sprite* a, const Sprite* b) {
		return a->texture->textureID < b->texture->textureID;
	}
	
	bool compareSpriteDepth(const Sprite* a, const Sprite* b) {
		return a->position.z < b->position.z;
	}

	void Renderer::end() {
		// Submit all the characters to be rendered this frame as sprites
		for (std::pair<Shader*, std::vector<Character>> text : m_text) {
			Shader* shader = text.first;
			std::vector<Sprite*> sprites(text.second.size());
			for (unsigned int i = 0; i < sprites.size(); i++) {
				sprites[i] = &(m_text[shader][i]);
			}
			submit(sprites, shader);
		}

		for (std::pair<Shader*, std::vector<Sprite*>> shaderBatch : m_sprites) {
			// Get the shader to render this batch of sprites with
			Shader* shader = shaderBatch.first;
			// Sort the sprites by their texture ID's
			if (m_sortType == SortType::TEXTURE) {
				std::stable_sort(m_sprites[shader].begin(),
					m_sprites[shader].end(), compareSpriteTexture);
			}
			else if (m_sortType == SortType::DEPTH) {
				std::stable_sort(m_sprites[shader].begin(),
					m_sprites[shader].end(), compareSpriteDepth);
			}
			// Start the shader
			shader->begin();

			// Initialize the data to send to OpenGL
			std::vector<float> vertexData;
			std::vector<unsigned int> indices;
			unsigned int spriteCount = 0;

			// Draw the sprites in this batch
			GLuint currentTextureID = 0;
			for (unsigned int i = 0; i < m_sprites[shader].size(); i++) {
				Sprite* sprite = m_sprites[shader][i];
				// Test if a new texture group has started
				if (currentTextureID != sprite->texture->textureID) {
					if (spriteCount > 0) {
						// If there were sprites in the last texture batch,
						// draw them
						drawVertices(vertexData, indices);
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
				for (unsigned int i : Sprite::SPRITE_INDICES) {
					indices.push_back(i + 4 * spriteCount);
				}
				spriteCount++;
			}

			// Draw the remaining vertex data from the final texture group
			drawVertices(vertexData, indices);

			// Stop this shader
			shader->end();
		}
	}

	void Renderer::drawVertices(const std::vector<float>& vertexData,
		const std::vector<unsigned int>& indices) {
		// Upload the vertex data to OpenGL
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * vertexData.size(),
			&(vertexData[0]), GL_STATIC_DRAW);
		// Upload the indices to OpenGL
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indices.size(),
			&(indices[0]), GL_STATIC_DRAW);

		// Draw the indices with OpenGL
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(),
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