/*
* File: Camera.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.14.2226
*/

#include <glm/gtc/matrix_transform.hpp>

#include "MW.h"
#include "Camera.h"

namespace MW {
	void Camera::init(const glm::ivec2& windowDimensions) {
		// Set the window dimensions and initialize the orthographic matrix for
		// those dimensions
		this->dimensions = windowDimensions;
		m_orthoMatrix = glm::ortho(
			0.0f, (float)windowDimensions.x,
			0.0f, (float)windowDimensions.y);
		updateMatrix();
	}

	void Camera::update(float deltaTime) {
		// Increment the position and scale of the camera by their velocities
		this->position += this->velocity * deltaTime;
		this->scale += this->scaleVelocity * deltaTime;

		// Check if the position or scale has changed since the last frame
		if (this->position != this->m_prevPosition
			|| this->scale != this->m_prevScale) {
			updateMatrix();
		}

		// Update the previous frame's position and scale
		this->m_prevPosition = this->position;
		this->m_prevScale = this->scale;
	}

	void Camera::updateMatrix() {
		// Rescale and reposition the camera matrix
		m_cameraMatrix = glm::translate(m_orthoMatrix,
			glm::vec3((float)this->dimensions.x / 2.0f,
				(float)this->dimensions.y / 2.0f, 0.0f));
		m_cameraMatrix = glm::scale(m_cameraMatrix,
			glm::vec3(this->scale, this->scale, 0.0f));
		m_cameraMatrix = glm::translate(m_cameraMatrix,
			glm::vec3((float)-this->dimensions.x / 2.0f,
				(float)-this->dimensions.y / 2.0f, 0.0f));
		m_cameraMatrix = glm::translate(m_cameraMatrix,
			glm::vec3(-this->position.x, -this->position.y, 0.0f));
	}
}