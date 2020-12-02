/*
* File: Camera.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.14.2225
*/

#pragma once

#include "Sprite.h"

namespace MW {
	/*
	* A 2D camera used to position sprites on the screen
	*/
	class Camera : public Sprite {
	public:
		// The scale of this camera
		float scale = 1.0f;
		// The velocity of the scale of this camera
		float scaleVelocity = 0.0f;

		/*
		* Initialize this camera with the window's dimensions
		*
		* @param windowDimensions: The dimensions of the window in pixels
		*/
		void init(const glm::ivec2& windowDimensions);
		/*
		* Update the position and scale of the camera
		*
		* @param deltaTime: The time elapsed since last frame
		*/
		void update(float deltaTime) override;
		/*
		* Get the camera's projection matrix to pass to a shader
		*/
		glm::mat4 getCameraMatrix() const { return m_cameraMatrix; }

	private:
		// The camera's blank projection matrix
		glm::mat4 m_orthoMatrix = glm::mat4();
		// The camera's full projection matrix
		glm::mat4 m_cameraMatrix = glm::mat4();
		// The position of this camera in the last frame
		glm::vec2 m_prevPosition = glm::vec2();
		// The scale of this camera in the last frame
		float m_prevScale = 1.0f;

		/*
		* Calculate this camera's projection matrix from the orthographic matrix
		* and the position and scale
		*/
		void updateMatrix();
	};
}