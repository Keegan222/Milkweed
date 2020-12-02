/*
* File: Sprite.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.15.1629
*/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace MW {
	/*
	* An abstract vertex with only a position
	*/
	struct Vertex {
		// The position of this vertex in screen coordinates
		glm::vec2 position;
	};

	/*
	* An abstract sprite with a position, velocity and dimensions
	*/
	class Sprite {
	public:
		// The position of this sprite
		glm::vec2 position = glm::vec2();
		// The velocity of this sprite
		glm::vec2 velocity = glm::vec2();
		// The dimensions of this sprite
		glm::vec2 dimensions = glm::vec2();

		/*
		* Update this sprite's physics
		* 
		* @param deltaTime: The time elapsed since the last update
		*/
		virtual void update(float deltaTime) = 0;
	};
}