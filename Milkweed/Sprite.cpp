/*
* File: Sprite.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.15
*/

#include "Sprite.h"

#define PI 3.141592f

namespace Milkweed {
	std::vector<unsigned int> Sprite::SPRITE_INDICES = {
		// Triangle 1
		0, 1, 2,
		// Triangle 2
		2, 3, 0,
	};

	void Sprite::init(const glm::vec3& position, const glm::vec2& dimensions,
		Texture* texture) {
		this->position = position;
		this->dimensions = dimensions;
		this->texture = texture;
	}

	void Sprite::update(float deltaTime) {
		// Increment the position of this sprite by its velocity
		position.x += (velocity.x * deltaTime);
		position.y += (velocity.y * deltaTime);
	}

	std::vector<float> Sprite::getVertexData() {
		glm::vec3 bl = glm::vec3(position.x, position.y, position.z);
		glm::vec3 br = glm::vec3(position.x + dimensions.x, position.y,
			position.z);
		glm::vec3 tr = glm::vec3(position.x + dimensions.x,
			position.y + dimensions.y, position.z);
		glm::vec3 tl = glm::vec3(position.x, position.y + dimensions.y,
			position.z);

		if (rotation % 360 != 0) {
			bl = rotatePoint(bl);
			br = rotatePoint(br);
			tr = rotatePoint(tr);
			tl = rotatePoint(tl);
		}

		// Generate the default set of vertex data
		std::vector<float> vertices = {
			// Vertex 1
			// Position
			bl.x, bl.y, bl.z,
			// Texture coordinates
			// 0.0f, 1.0f,
			textureCoords.x, textureCoords.y + textureCoords.w,

			// Vertex 2
			// Position
			br.x, br.y, br.z,
			// Texture coordinates
			// 1.0f, 1.0f,
			textureCoords.x + textureCoords.z, textureCoords.y + textureCoords.w,

			// Vertex 3
			// Position
			tr.x, tr.y, tr.z,
			// Texture coordinates
			// 1.0f, 0.0f,
			textureCoords.x + textureCoords.z, textureCoords.y,

			// Vertex 4
			// Position
			tl.x, tl.y, tl.z,
			// Texture coordinates
			// 0.0f, 0.0f,
			textureCoords.x, textureCoords.y,
		};

		// Flip vertex positions of needed
		flip(&vertices);

		return vertices;
	}

	bool Sprite::intersects(Sprite* sprite) {
		if (position.x > sprite->position.x + sprite->dimensions.x
			|| position.x + dimensions.x < sprite->position.x) {
			// This sprite is outside the other on the x-axis
			return false;
		}
		if (position.y > sprite->position.y + sprite->dimensions.y
			|| position.y + dimensions.y < sprite->position.y) {
			// This sprite is outside the other on the y-axis
			return false;
		}

		// The sprites intersect
		return true;
	}

	void Sprite::destroy() {
		position = glm::vec3();
		velocity = glm::vec2();
		dimensions = glm::vec2();
		texture = nullptr;
		textureCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		rotation = 0;
		flipHorizontal = flipVertical = false;
	}

	void Sprite::flip(std::vector<float>* vertices) {
		if (flipHorizontal) {
			// Flip the positions of the vertices horizontally
			swapElements(vertices, 3, 8);
			swapElements(vertices, 13, 18);
		}
		if (flipVertical) {
			// Flip the positions of the vertices vertically
			swapElements(vertices, 4, 19);
			swapElements(vertices, 9, 14);
		}
	}

	glm::vec3 Sprite::rotatePoint(const glm::vec3& p) {
		float angle = (PI / 180.0f) * (float)rotation;
		glm::vec2 c = glm::vec2(position.x, position.y) + (dimensions / 2.0f);
		float rx = cos(angle) * (p.x - c.x) - sin(angle) * (p.y - c.y) + c.x;
		float ry = sin(angle) * (p.x - c.x) + cos(angle) * (p.y - c.y) + c.y;
		return glm::vec3(rx, ry, p.z);
	}

	void Sprite::swapElements(std::vector<float>* v, unsigned int a,
		unsigned int b) {
		// Swap the element of v at a with the element at b
		float s = v->at(a);
		v->at(a) = v->at(b);
		v->at(b) = s;
	}

	void AnimatedSprite::init(const glm::vec3& position,
		const glm::vec2& dimensions, Texture* texture,
		const glm::ivec2& frameDimensions, float frameTime) {
		// Initialize as a sprite first
		((Sprite*)this)->init(position, dimensions, texture);

		// Get the size of each frame in texture space
		m_frameSize = glm::vec2(
			1.0f / (float)frameDimensions.x,
			1.0f / (float)frameDimensions.y);

		// Set the texture coordinates of the origin of each frame in texture
		// space
		m_frames.resize(frameDimensions.x * frameDimensions.y);
		for (int y = 0; y < frameDimensions.y; y++) {
			for (int x = 0; x < frameDimensions.x; x++) {
				m_frames[x + (y * frameDimensions.x)]
					= glm::vec2(m_frameSize.x * x, m_frameSize.y * y);
			}
		}

		// Set the frame time
		m_frameTime = frameTime;
	}
	
	void AnimatedSprite::update(float deltaTime) {
		// Increment the position of this sprite by the velocity
		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;

		// If the animation is not playing there is no need to proceed
		if (!m_playing) {
			return;
		}

		if (m_timer > m_frameTime) {
			// Increment the frame or loop back to the beginning
			m_timer = 0.0f;
			m_frame = (m_frame + 1) % m_frames.size();
		}
		m_timer += deltaTime;

		textureCoords = glm::vec4(m_frames[m_frame].x, m_frames[m_frame].y,
			m_frameSize.x, m_frameSize.y);
	}

	void AnimatedSprite::play() {
		m_playing = true;
	}

	void AnimatedSprite::pause() {
		m_playing = false;
	}

	void AnimatedSprite::stop() {
		m_playing = false;
		m_frame = 0;
		m_timer = 0.0f;
	}
}