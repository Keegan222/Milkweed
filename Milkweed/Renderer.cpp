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
#include "Resources.h"

namespace MW {
	Renderer Renderer::m_instance;

	void Renderer::init() {
		// Give the window the OpenGL context
		glfwMakeContextCurrent(App::WINDOW.getWindowHandle());

		// Initialize GLEW and get the running OpenGL version
		if (glewInit() != GLEW_OK) {
			// GLEW could not be initialize
			App::LOG << "Failed to initialize GLEW\n";
			return;
		}

		const GLubyte* version = glGetString(GL_VERSION);
		App::LOG << "OpenGL version: " << version << "\n";

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

			ch.position.x = x + fc.bearing.x * scale;
			ch.position.y = position.y - (fc.dimensions.y - fc.bearing.y)
				* scale;
			ch.position.z = position.z;
			ch.dimensions = fc.dimensions * scale;
			x += (fc.offset >> 6) * scale;
			ch.texture = &(font->characters[c].texture);
			
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
			App::LOG << "NEW FRAME DUMP BEGIN\n";
		}

		// Submit all the characters to be rendered this frame as sprites
		for (std::pair<Shader*, std::vector<Sprite>> text : m_text) {
			if (m_dumpFrame) {
				App::LOG << "New text shader found\n";
			}
			Shader* shader = text.first;
			std::vector<Sprite*> sprites(text.second.size());
			for (unsigned int i = 0; i < sprites.size(); i++) {
				if (m_dumpFrame) {
					App::LOG << "Adding pointer to character sprite " << i << "\n";
				}
				sprites[i] = &(m_text[shader][i]);
			}
			if (m_dumpFrame) {
				App::LOG << "Submitting shader batch of " << sprites.size()
					<< " character sprites\n";
			}
			submit(sprites, shader);
		}

		for (std::pair<Shader*, std::vector<Sprite*>> shaderBatch : m_sprites) {
			if (m_dumpFrame) {
				App::LOG << "New sprite shader batch found\n";
			}
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
			if (m_dumpFrame) {
				App::LOG << "Started shader\n";
			}

			// Initialize the data to send to OpenGL
			std::vector<float> vertexData;
			std::vector<unsigned int> indices;
			unsigned int spriteCount = 0;

			if (m_dumpFrame) {
				App::LOG << "Sprites sorted by ";
				if (m_sortType == SortType::TEXTURE) {
					App::LOG << "texture\n";
				}
				else if (m_sortType == SortType::DEPTH) {
					App::LOG << "depth\n";
				}
			}

			// Draw the sprites in this batch
			GLuint currentTextureID = 0;
			for (unsigned int i = 0; i < m_sprites[shader].size(); i++) {
				if (m_dumpFrame) {
					App::LOG << "Testing sprite " << i << "'s texture ID\n";
				}
				Sprite* sprite = m_sprites[shader][i];
				// Test if a new texture group has started
				if (currentTextureID != sprite->texture->textureID) {
					if (m_dumpFrame) {
						App::LOG << "Sprite " << i << " has a new texture ID\n";
					}
					if (spriteCount > 0) {
						if (m_dumpFrame) {
							App::LOG << "There were sprites drawn with the "
								<< "last texture ID, render their vertices\n";
						}
						// If there were sprites in the last texture batch,
						// draw them
						drawVertices(vertexData, indices);
					}

					if (m_dumpFrame) {
						App::LOG << "Binding the new texture ID "
							<< sprite->texture->textureID << " and clearing "
							<< "old vertex and index data\n";
					}
					// Reset the vertex data and bind the new texture
					currentTextureID = sprite->texture->textureID;
					glBindTexture(GL_TEXTURE_2D, currentTextureID);
					vertexData.clear();
					indices.clear();
					spriteCount = 0;
				}

				if (m_dumpFrame) {
					App::LOG << "Adding sprite " << i << "'s vertex and index"
						<< " data to the new texture frame\n";
				}
				// Add this sprite's data to the current texture group's vertex
				// data to be uploaded to OpenGL
				if (m_dumpFrame) {
					App::LOG << "Pushing new vertex data:\n";
				}
				for (float f : sprite->getVertexData()) {
					if (m_dumpFrame) {
						App::LOG << f << ", ";
					}
					vertexData.push_back(f);
				}
				if (m_dumpFrame) {
					App::LOG << "\nPushing new index data:\n";
				}
				for (unsigned int i : Sprite::SPRITE_INDICES) {
					indices.push_back(i + 4 * spriteCount);
					if (m_dumpFrame) {
						App::LOG << (i + 4 * spriteCount) << ", ";
					}
				}
				if (m_dumpFrame) {
					App::LOG << "\n";
				}
				spriteCount++;
			}

			if (m_dumpFrame) {
				App::LOG << "Drawing remaning vertices and indices and clearing"
					<< " vertex and index buffers\n";
			}
			// Draw the remaining vertex data from the final texture group
			drawVertices(vertexData, indices);
			vertexData.clear();
			indices.clear();

			if (m_dumpFrame) {
				App::LOG << "Stopping shader\n";
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
			App::LOG << "Drawing " << vertexData.size() << " float vertex data"
				<< " points with " << indices.size() << " indices using "
				<< "glDrawElements\n";
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
	}
}