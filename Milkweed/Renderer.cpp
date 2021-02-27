/*
* File: Renderer.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.01
*/

#include <algorithm>
#include <GL/glew.h>

#include "MW.h"

namespace Milkweed {
	Renderer Renderer::m_instance;

	bool Renderer::init() {
		// Give the window the OpenGL context
		glfwMakeContextCurrent(MW::WINDOW.getWindowHandle());

		// Initialize GLEW and get the running OpenGL version
		if (glewInit() != GLEW_OK) {
			// GLEW could not be initialize
			MWLOG(Error, Renderer, "Failed to initialize GLEW");
			return false;
		}

		const GLubyte* version = glGetString(GL_VERSION);
		MWLOG(Info, Renderer, "Initialized GLEW, OpenGL version: ", version);

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

		return true;
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

	void Renderer::submit(const std::string& text, const glm::vec3& position,
		const glm::vec4& bounds, float scale, const glm::vec3& color,
		Font* font, Shader* shader) {
		// Attempt to find the shader in the text map
		std::unordered_map<Shader*, std::vector<Sprite>>::iterator it
			= m_text.find(shader);
		if (it == m_text.end()) {
			// Not found, add one
			m_text[shader] = std::vector<Sprite>(0);
		}

		float x = position.x;
		for (char c : text) {
			const Character& fc = font->characters[c];
			Sprite ch;

			// Set the position, dimensions, and texture of this character and
			// advance the offset to the start of the next character in the
			// string
			ch.init(glm::vec3(x + fc.bearing.x * scale,
				position.y - (fc.dimensions.y - fc.bearing.y) * scale,
				position.z), fc.dimensions * scale,
				&(font->characters[c].texture));
			x += (fc.offset >> 6) * scale;
			
			if (ch.position.x >= bounds.x
				&& ch.position.y >= bounds.y
				&& ch.position.x + ch.dimensions.x
					<= bounds.x + bounds.z
				&& ch.position.y + ch.dimensions.y
					<= bounds.y + bounds.w) {
				// Submit this character for rendering if it is in the bounds
				// of the label
				m_text[shader].push_back(ch);
			}
		}
	}

	bool compareSpriteTexture(const Sprite* a, const Sprite* b) {
		return a->texture->textureID < b->texture->textureID;
	}
	
	bool compareSpriteDepth(const Sprite* a, const Sprite* b) {
		return a->position.z < b->position.z;
	}

	void Renderer::end() {
		if (m_dumpFrame) {
			MWLOG(Info, Renderer, "Renderer frame info dump:");
		}

		// Submit all the characters to be rendered this frame as sprites
		for (std::pair<Shader*, std::vector<Sprite>> text : m_text) {
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "New text shader found");
			}
			Shader* shader = text.first;
			std::vector<Sprite*> sprites(text.second.size());
			for (unsigned int i = 0; i < sprites.size(); i++) {
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Adding pointer to character sprite ",
						i);
				}
				sprites[i] = &(m_text[shader][i]);
			}
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Submitting shader batchs of ",
					sprites.size(), " character sprites");
			}
			submit(sprites, shader);
		}

		for (std::pair<Shader*, std::vector<Sprite*>> shaderBatch : m_sprites) {
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "New sprite shader batch found");
			}
			// Get the shader to render this batch of sprites with
			Shader* shader = shaderBatch.first;
			// Sort the sprites by their texture ID's
			if (m_sortType == SortType::TEXTURE) {
				std::stable_sort(m_sprites[shader].begin(),
					m_sprites[shader].end(), compareSpriteTexture);
				MWLOG(Info, Renderer, "Sorted sprites by texture");
			}
			else if (m_sortType == SortType::DEPTH) {
				std::stable_sort(m_sprites[shader].begin(),
					m_sprites[shader].end(), compareSpriteDepth);
				MWLOG(Info, Renderer, "Sorted sprites by depth");
			}
			// Start the shader
			shader->begin();
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Started shader");
			}

			// Initialize the data to send to OpenGL
			std::vector<float> vertexData;
			std::vector<unsigned int> indices;
			unsigned int spriteCount = 0;

			// Draw the sprites in this batch
			GLuint currentTextureID = 0;
			for (unsigned int i = 0; i < m_sprites[shader].size(); i++) {
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Testing sprite ", i, "'s texture ID");
				}
				Sprite* sprite = m_sprites[shader][i];
				// Test if a new texture group has started
				if (currentTextureID != sprite->texture->textureID) {
					if (m_dumpFrame) {
						MWLOG(Info, Renderer, "Sprite ", i, " has a new ",
							"texture ID");
					}
					if (spriteCount > 0) {
						if (m_dumpFrame) {
							MWLOG(Info, Renderer, "There were sprites drawn ",
								"with the last texture ID, render out their ",
								"vertices");
						}
						// If there were sprites in the last texture batch,
						// draw them
						drawVertices(vertexData, indices);
					}

					if (m_dumpFrame) {
						MWLOG(Info, Renderer, "Binding new texture ID ",
							sprite->texture->textureID, " and clearing out ",
							"old vertex data and indices");
					}
					// Reset the vertex data and bind the new texture
					currentTextureID = sprite->texture->textureID;
					glBindTexture(GL_TEXTURE_2D, currentTextureID);
					vertexData.clear();
					indices.clear();
					spriteCount = 0;
				}

				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Adding sprite ", i, "'s vertex and ",
						"index data to the new texture frame");
				}
				// Add this sprite's data to the current texture group's vertex
				// data to be uploaded to OpenGL
				unsigned int count = 0;
				for (float f : sprite->getVertexData()) {
					vertexData.push_back(f);
					count++;
				}
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Pushed ", count, " new vertex data ",
						"points");
				}
				count = 0;
				for (unsigned int i : Sprite::SPRITE_INDICES) {
					indices.push_back(i + 4 * spriteCount);
					count++;
				}
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Pushed ", count, " new index data ",
						"points");
				}
				spriteCount++;
			}

			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Drawing remaining vertices and indices ",
					"and clearing the frame's vertex data");
			}
			// Draw the remaining vertex data from the final texture group
			drawVertices(vertexData, indices);
			vertexData.clear();
			indices.clear();

			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Stopping shader batch");
			}
			// Stop this shader
			shader->end();
		}

		if (m_dumpFrame) {
			m_dumpFrame = false;
		}
	}

	void Renderer::drawVertices(const std::vector<float>& vertexData,
		const std::vector<unsigned int>& indices) {
		if (m_dumpFrame) {
			MWLOG(Info, Renderer, "Drawing ", vertexData.size(), " float ",
				"vertex data points with ", indices.size(), " indices using ",
				"glDrawElements");
		}
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

		MWLOG(Info, Renderer, "Destroying renderer");
	}

	void Renderer::setClearColor(const glm::vec3& clearColor) {
		m_clearColor = clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
	}
}