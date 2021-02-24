/*
* File: Sprite.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.15.1629
*/

#ifndef MW_SPRITE_H
#define MW_SPRITE_H

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace MW {
	// Declare the Texture class from ResourceManager here
	struct Texture;

	/*
	* An abstract sprite with a position, velocity and dimensions
	*/
	class Sprite {
	public:
		// The indices for a single-quad sprite
		static std::vector<unsigned int> SPRITE_INDICES;

		// The position of this sprite
		glm::vec3 position = glm::vec3();
		// The velocity of this sprite
		glm::vec2 velocity = glm::vec2();
		// The dimensions of this sprite
		glm::vec2 dimensions = glm::vec2();
		// The texture for this sprite
		Texture* texture = nullptr;
		// Whether the sprite's texture should be flipped horizontally
		bool flipHorizontal = false;
		// Whether the sprite's texture should be flipped vertically
		bool flipVertical = false;

		/*
		* Construct a blank sprite
		*/
		Sprite() {}
		/*
		* Construct a sprite with position, dimensions, and texture
		* 
		* @param position: The position of this sprite
		* @param dimensions: The dimensions of this sprite
		* @param texture: A pointer to this sprite's texture
		*/
		Sprite(const glm::vec3& Position, const glm::vec2& Dimensions,
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

	protected:
		/*
		* Swap the position of texture coordinates in vertex data of animated
		* and static sprites
		* 
		* @param vertices: The vertex data of the animated or static sprite
		*/
		void flip(std::vector<float>* vertices);

	private:
		// Swap the position of two elements of a std::vector
		void swapElements(std::vector<float>* v, unsigned int a,
			unsigned int b);
	};

	/*
	* A sprite which plays an animation
	*/
	class AnimatedSprite : public Sprite {
	public:
		/*
		* Construct blank animated sprite
		*/
		AnimatedSprite() {}
		/*
		* Construct a new animated sprite as a blank sprite
		* 
		* @param position: The position of this animated sprite
		* @param dimensions: The dimensions of this animated sprite
		* @param texture: A pointer to the texture containing the frames of this
		* sprite's animation
		* @param frameDimensions: The dimensions of the texture for this sprite
		* in frames
		* @param frameTime: The number of updates each frame of the animation
		* should be displayed for
		*/
		AnimatedSprite(const glm::vec3& position, const glm::vec2& dimensions,
			Texture* texture, const glm::ivec2& frameDimensions,
			float frameTime);
		/*
		* Update this sprite's physics and animation
		* 
		* @param deltaTime: The time elapsed since the last update
		*/
		void update(float deltaTime) override;
		/*
		* Let the animation play from the current frame
		*/
		void play() { m_playing = true; }
		/*
		* Stop playing the animation on the current frame
		*/
		void pause() { m_playing = false; }
		/*
		* Stop playing the animation and reset it to the first frame
		*/
		void stop() {
			m_playing = false;
			m_frame = 0;
			m_timer = 0.0f;
		}
		/*
		* Get whether this sprite's animation is currently playing
		*/
		bool isPlaying() const { return m_playing; }
		/*
		* Get the vertex data of this sprite to pass to OpenGL for rendering
		* 
		* @return The array of vertices making up this sprite
		*/
		std::vector<float> getVertexData() override;
		
	private:
		// The time for which to display each frame of the animation
		float m_frameTime = 0.0f;
		// The texture coordinates of the origin of each frame of the animation
		std::vector<glm::vec2> m_frames;
		// The size of the frames in texture space
		glm::vec2 m_frameSize = glm::vec2();
		// The current frame of the animation being displayed
		unsigned int m_frame = 0;
		// Whether the animation is current playing or not
		bool m_playing = true;
		// The timer for the animation
		float m_timer = 0.0f;
	};
}

#endif