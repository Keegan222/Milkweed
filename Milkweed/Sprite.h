/*
* File: Sprite.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.15.1629
*/

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ResourceManager.h"

namespace MW {
	/*
	* An abstract sprite with a position, velocity and dimensions
	*/
	class Sprite {
	public:
		// The indices for the vertices of any sprite
		static std::vector<unsigned int> VERTEX_INDICES;

		// The position of this sprite
		glm::vec2 position = glm::vec2();
		// The velocity of this sprite
		glm::vec2 velocity = glm::vec2();
		// The dimensions of this sprite
		glm::vec2 dimensions = glm::vec2();
		// The texture for this sprite
		Texture* texture = &ResourceManager::NO_TEXTURE;

		/*
		* Construct a blank sprite
		*/
		Sprite() {}
		/*
		* Construct a sprite with position, dimensions, and texture
		*/
		Sprite(const glm::vec2& Position, const glm::vec2& Dimensions,
			Texture* Texture) : position(Position), dimensions(Dimensions),
			texture(Texture) {}
		/*
		* Update this sprite's physics
		* 
		* @param deltaTime: The time elapsed since the last update
		*/
		virtual void update(float deltaTime);
		/*
		* Get the vertex data of this sprite to pass to OpenGL for rendering
		* 
		* @return The array of vertices making up this sprite
		*/
		virtual std::vector<float> getVertexData();
	};
}