/*
* File: Camera.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.14.2226
*/

#include <glm/gtc/matrix_transform.hpp>

#include "MW.h"
#include "Camera.h"

namespace MW {
	void Camera::init() {
		// Set the window dimensions and initialize the orthographic matrix for
		// those dimensions
		updateMatrix();
	}

	void Camera::update(float deltaTime) {
		// Increment the position and scale of the camera by their velocities
		this->position.x += this->velocity.x * deltaTime;
		this->position.y += this->velocity.y * deltaTime;
		this->scale += this->scaleVelocity * deltaTime;

		// Check if the position or scale has changed since the last frame
		if (this->position != this->m_prevPosition
			|| this->scale != this->m_prevScale
			|| this->m_prevWindowDimensions != App::WINDOW.getDimensions()) {
			updateMatrix();
		}

		// Update the previous frame's position and scale
		this->m_prevPosition = this->position;
		this->m_prevScale = this->scale;
		this->m_prevWindowDimensions = App::WINDOW.getDimensions();
	}

	void Camera::updateMatrix() {
		App::LOG << "Update camera resolution ("
			<< App::WINDOW.getDimensions().x << ", "
			<< App::WINDOW.getDimensions().y << ")\n";
		// Rescale and reposition the camera matrix
		m_orthoMatrix = glm::ortho(
			0.0f, (float)App::WINDOW.getDimensions().x,
			0.0f, (float)App::WINDOW.getDimensions().y);
		m_cameraMatrix = glm::translate(m_orthoMatrix,
			glm::vec3(-position.x + (float)App::WINDOW.getDimensions().x / 2,
				-position.y + (float)App::WINDOW.getDimensions().y / 2, 0.0f));
		m_cameraMatrix = glm::scale(glm::mat4(1.0f),
			glm::vec3(scale, scale, 0.0f)) * m_cameraMatrix;
	}
}