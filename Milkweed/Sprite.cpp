/*
* File: Sprite.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.15.1651
*/

#include "Sprite.h"

namespace MW {
	std::vector<unsigned int> Sprite::VERTEX_INDICES = {
		// Triangle 1
		0, 1, 2,
		// Triangle 2
		2, 3, 0,
	};

	void Sprite::update(float deltaTime) {
		position += velocity * deltaTime;
	}

	std::vector<float> Sprite::getVertexData() {
		return {
			// Vertex 1
			position.x, position.y,
			0.0f, 1.0f,

			// Vertex 2
			position.x + dimensions.x, position.y,
			1.0f, 1.0f,

			// Vertex 3
			position.x + dimensions.x, position.y + dimensions.y,
			1.0f, 0.0f,

			// Vertex 4
			position.x, position.y + dimensions.y,
			0.0f, 0.0f,
		};
	}
}