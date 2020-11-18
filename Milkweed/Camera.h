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
		void update(float deltaTime);
		/*
		* Get the camera's projection matrix to pass to a shader
		*/
		glm::mat4 getCameraMatrix() const { return m_cameraMatrix; }
		/*
		* Get the current scale of the camera
		*/
		float getScale() const { return m_scale; }
		/*
		* Set the current scale of the camera
		* 
		* @param scale: The new scale of the camera
		*/
		void setScale(float scale) {
			m_scale = scale;
			m_needsUpdate = true;
		}
		/*
		* Get the current scale velocity of the camera
		*/
		float getScaleVelocity() const { return m_scaleVelocity; }
		/*
		* Set the current scale velocity of the camera
		*
		* @param scaleVelocity: The new scale velocity of the camera
		*/
		void setScaleVelocity(float scaleVelocity) {
			m_scaleVelocity = scaleVelocity;
			m_needsUpdate = true;
		}

	private:
		// The camera's blank projection matrix
		glm::mat4 m_orthoMatrix = glm::mat4();
		// The camera's full projection matrix
		glm::mat4 m_cameraMatrix = glm::mat4();
		// The current scale of the camera
		float m_scale = 1.0f;
		// The velocity of the scale on the camera this frame
		float m_scaleVelocity = 0.0f;
	};
}