/*
* File: Camera.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.14.2226
*/

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

namespace MW {
	void Camera::init(const glm::ivec2& windowDimensions) {
		// Set the window dimensions and initialize the orthographic matrix for
		// those dimensions
		m_dimensions = windowDimensions;
		m_orthoMatrix = glm::ortho(
			0.0f, (float)windowDimensions.x,
			0.0f, (float)windowDimensions.y);
	}

	void Camera::update(float deltaTime) {
		if (m_needsUpdate) {
			// Rescale and reposition the camera matrix
			m_cameraMatrix = glm::translate(m_orthoMatrix,
				glm::vec3((float)m_dimensions.x / 2.0f,
					(float)m_dimensions.y / 2.0f, 0.0f));
			m_cameraMatrix = glm::scale(m_cameraMatrix,
				glm::vec3(m_scale, m_scale, 0.0f));
			m_cameraMatrix = glm::translate(m_cameraMatrix,
				glm::vec3((float)-m_dimensions.x / 2.0f,
					(float)-m_dimensions.y / 2.0f, 0.0f));
			m_cameraMatrix = glm::translate(m_cameraMatrix,
				glm::vec3(-m_position.x, -m_position.y, 0.0f));

			// The camera matrix is updated, and doesn't have to be on the next
			// frame
			m_needsUpdate = false;
		}

		m_position += (m_velocity / m_scale) * deltaTime;
		m_scale += m_scaleVelocity * deltaTime;
	}
}