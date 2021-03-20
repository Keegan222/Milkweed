/*
* File: Renderer.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.01
*/

#include <algorithm>
#include <cstdlib>
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
	}

	void Renderer::submit(const std::vector<Sprite*>& sprites, Shader* shader) {
		// If there are no sprites or shader do nothing
		if (sprites.empty() || shader == nullptr) {
			return;
		}

		// Add the sprites to their shader
		for (Sprite* sprite : sprites) {
			// Make sure the sprite has a texture, not nullptr
			if (sprite->texture == nullptr) {
				continue;
			}
			// Add the sprite to the frame
			sprite->m_shader = shader;
			m_sprites.push_back(sprite);
		}
	}

	void Renderer::submit(const std::string& text, const glm::vec3& position,
		const glm::vec4& bounds, float scale, Font* font, Shader* shader,
		Justification hJustification, Justification vJustification) {
		// Make sure text has been submitted
		if (text.empty()) {
			return;
		}

		// Attempt to find the shader in the text map
		std::unordered_map<Shader*, std::vector<Sprite>>::iterator it
			= m_text.find(shader);
		if (it == m_text.end()) {
			// Not found, add one
			m_text[shader] = std::vector<Sprite>(0);
		}

		// Calculate the width and height of the text clamped to the boundaries
		// of the characters
		float labelWidth = 0.0f, max = font->maxCharacterHeight * scale,
			min = 0.0f, labelHeight = 0.0f;
		for (char c : text) {
			labelWidth += font->characters[c].offset * scale;
			if (min > -(font->characters[c].dimensions.y
				- font->characters[c].bearing.y) * scale) {
				min = -(font->characters[c].dimensions.y
					- font->characters[c].bearing.y) * scale;
			}
		}
		labelHeight = max - min;

		// Position the text in the x-axis
		float x;
		switch (hJustification) {
		case Justification::LEFT: {
			x = position.x + scale;
			break;
		}
		case Justification::CENTER: {
			x = position.x + ((bounds.z - labelWidth) / 2);
			break;
		}
		case Justification::RIGHT: {
			x = position.x + bounds.z- labelWidth - scale;
			break;
		}
		default: {
			x = position.x + scale;
			break;
		}
		}

		// Position the text in the y-axis
		float y;
		switch (vJustification) {
		case Justification::TOP: {
			y = position.y + bounds.w - max;
			break;
		}
		case Justification::CENTER: {
			y = position.y + ((bounds.w - labelHeight) / 2.0f);
			break;
		}
		case Justification::BOTTOM: {
			y = position.y;
			break;
		}
		default:
			y = position.y;
			break;
		}

		// Add the text to the frame as a set of sprites representing each
		// character
		int count = 0;
		for (char c : text) {
			const Character& fc = font->characters[c];
			Sprite ch;

			// Set the position, dimensions, and texture of this character and
			// advance the offset to the start of the next character in the
			// string
			ch.init(glm::vec3(x + fc.bearing.x * scale,
				y - ((fc.dimensions.y - fc.bearing.y) * scale),
				position.z), fc.dimensions * scale,
				&(font->characters[c].texture));
			x += fc.offset * scale;
			
			// Add the character to the frame if it is inside the bounds
			if (ch.position.x >= bounds.x
				&& ch.position.x + ch.dimensions.x <= bounds.x + bounds.z
				&& ch.position.y >= bounds.y
				&& ch.position.y + ch.dimensions.y <= bounds.y + bounds.w) {
				m_text[shader].push_back(ch);
			}
			else {
				MWLOG(Info, Renderer, "Rejected character ", c, " at (",
					ch.position.x, ", ", ch.position.y, ", ",
					ch.dimensions.x, ", ", ch.dimensions.y, ")");
			}
		}
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

		// Sort all the sprites by their depth
		std::stable_sort(m_sprites.begin(), m_sprites.end(), compareSpriteDepth);
		if (m_dumpFrame) {
			MWLOG(Info, Renderer, "Sorted sprites by depth");
		}

		// Initialize the sprite data
		std::vector<float> vertexData;
		std::vector<unsigned int> indices;
		unsigned int spriteCount = 0;

		Shader* shader = nullptr;
		// Bind the first texture
		GLuint currentTextureID = m_sprites[0]->texture->textureID;
		glBindTexture(GL_TEXTURE_2D, currentTextureID);
		// Go through the sprites in this frame
		for (Sprite* sprite : m_sprites) {
			// Check if there is a new shader
			if (shader != sprite->m_shader) {
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "New shader group found");
				}
				// New shader, draw out all current vertices and change shaders
				if (shader != nullptr) {
					if (spriteCount > 0) {
						if (m_dumpFrame) {
							MWLOG(Info, Renderer, spriteCount, " sprites were ",
								"drawn with the last shader and must be ",
								"rendered out");
						}
						drawVertices(vertexData, indices);
						vertexData.clear();
						indices.clear();
					}
					shader->end();
				}
				// Start the new shader
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Starting new shader group");
				}
				shader = sprite->m_shader;
				shader->begin();
				spriteCount = 0;
			}
			// Check if there is a new texture
			if (currentTextureID != sprite->texture->textureID) {
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "New texture ID found, ",
						sprite->texture->textureID);
				}
				// A new texture was found, draw out all old sprites
				if (spriteCount > 0) {
					if (m_dumpFrame) {
						MWLOG(Info, Renderer, "Sprites were drawn with the ",
							"last texture ID and must be rendered out");
					}
					drawVertices(vertexData, indices);
					vertexData.clear();
					indices.clear();
				}
				// Update the texture ID
				if (m_dumpFrame) {
					MWLOG(Info, Renderer, "Starting new texture ID group");
				}
				currentTextureID = sprite->texture->textureID;
				glBindTexture(GL_TEXTURE_2D, currentTextureID);
				spriteCount = 0;
			}

			// Add the next sprite's data to the group
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Adding new sprite vertex and ",
					"index data to the new texture frame");
			}
			int count = 0;
			for (float f : sprite->getVertexData()) {
				vertexData.push_back(f);
				count++;
			}
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Added ", count, " vertex data points");
			}
			count = 0;
			for (unsigned int i : Sprite::SPRITE_INDICES) {
				indices.push_back(i + 4 * spriteCount);
				count++;
			}
			if (m_dumpFrame) {
				MWLOG(Info, Renderer, "Added ", count, " indices");
			}
			spriteCount++;
		}

		// Draw the last of the data for this frame
		if (m_dumpFrame) {
			MWLOG(Info, Renderer, "Rendering out remaining vertex data ",
				"and ending the frame");
		}
		drawVertices(vertexData, indices);
		shader->end();
		vertexData.clear();
		indices.clear();
		m_sprites.clear();
		m_text.clear();

		if (m_dumpFrame) {
			MWLOG(Info, Renderer, "Renderer frame info dump complete");
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