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
		/*
		* Update this sprite's physics
		* 
		* @param deltaTime: The time elapsed since the last update
		*/
		virtual void update(float deltaTime) = 0;
		/*
		* Get the vertex data of this sprite
		* 
		* @return The vertices of this sprite in window-coordinates
		*/
		virtual Vertex* getVertices() = 0;
		/*
		* @return The current position of this sprite
		*/
		glm::vec2 getPosition() const { return m_position; }
		/*
		* Set the position of this sprite
		* 
		* @param position: The new position of this sprite
		*/
		void setPosition(const glm::vec2& position) {
			m_position = position;
			m_needsUpdate = true;
		}
		/*
		* Set the position of this sprite on the x-axis
		* 
		* @param positionX: The new position of this sprite on the x-axis
		*/
		void setPositionX(float positionX) {
			m_position.x = positionX;
			m_needsUpdate = true;
		}
		/*
		* Set the position of this sprite on the y-axis
		* 
		* @param positionY: The new position of this sprite on the y-axis
		*/
		void setPositionY(float positionY) {
			m_position.y = positionY;
			m_needsUpdate = true;
		}
		/*
		* @return The current velocity of this sprite
		*/
		glm::vec2 getVelocity() const { return m_velocity; }
		/*
		* Set the velocity of this sprite
		*
		* @param velocity: The new velocity of this sprite
		*/
		void setVelocity(const glm::vec2& velocity) {
			m_velocity = velocity;
			m_needsUpdate = true;
		}
		/*
		* Set the velocity of this sprite on the x-axis
		*
		* @param velocityX: The new velocity of this sprite on the x-axis
		*/
		void setVelocityX(float velocityX) {
			m_velocity.x = velocityX;
			m_needsUpdate = true;
		}
		/*
		* Set the velocity of this sprite on the y-axis
		*
		* @param velocityY: The new velocity of this sprite on the y-axis
		*/
		void setVelocityY(float velocityY) {
			m_velocity.y = velocityY;
			m_needsUpdate = true;
		}
		/*
		* Get the current dimensions of this sprite
		*/
		glm::vec2 getDimensions() const { return m_dimensions; }
		/*
		* Set the dimensions of this sprite
		*
		* @param dimensions: The new dimensions of this sprite
		*/
		void setDimensions(const glm::vec2& dimensions) {
			m_dimensions = dimensions;
			m_needsUpdate = true;
		}
		/*
		* Set the width of this sprite
		*
		* @param width: The new dimensions of this sprite on the x-axis
		*/
		void setWidth(float width) {
			m_dimensions.x = width;
			m_needsUpdate = true;
		}
		/*
		* Set the height of this sprite
		*
		* @param height: The new dimensions of this sprite on the y-axis
		*/
		void setHeight(float height) {
			m_dimensions.y = height;
			m_needsUpdate = true;
		}

	protected:
		// The position of this sprite
		glm::vec2 m_position = glm::vec2();
		// The velocity of this sprite
		glm::vec2 m_velocity = glm::vec2();
		// The dimensions of this sprite
		glm::vec2 m_dimensions = glm::vec2();
		// Whether the vertices of this sprite must be fixed next update
		bool m_needsUpdate = true;
	};
}