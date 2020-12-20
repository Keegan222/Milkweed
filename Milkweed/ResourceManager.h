/*
* File: ResourceManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.22.1939
*/

#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <AL/al.h>

extern int decodePNG(std::vector<unsigned char>& out_image,
	unsigned long& image_width, unsigned long& image_height,
	const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);

namespace MW {
	/*
	* A wrapper for the ID of an OpenGL texture
	*/
	struct Texture {
		// The OpenGL ID of this texture
		GLuint textureID = 0;
		// The width of this texture in pixels
		unsigned int width = 0;
		// The height of this texture in pixels
		unsigned int height = 0;

		/*
		* Make a blank texture with no ID, width, or height
		*/
		Texture() : textureID(0), width(0), height(0) {}
		/*
		* Make a texture with the given ID, width, and height
		*/
		Texture(GLuint TextureID, unsigned int Width, unsigned int Height) :
			textureID(TextureID), width(Width), height(Height) {}
	};

	/*
	* A wrapper for an OpenAL buffer ID
	*/
	struct Sound {
		// The ID number of this sound in OpenAL
		ALuint soundID = 0;

		/*
		* Make a blank sound with no ID
		*/
		Sound() {}
		/*
		* Make a sound with the given ID
		*/
		Sound(ALuint SoundID) : soundID(SoundID) {}
	};

	/*
	* The Milkweed engine's utility for loading resources (textures and sound
	* effects) into the application
	*/
	class ResourceManager {
	public:
		// An empty texture to initialize sprites with
		static Texture* NO_TEXTURE;
		// An empty sound to initialize objects with
		static Sound* NO_SOUND;

		/*
		* Get a texture from memory or the disk
		* 
		* @param fileName: The file name of the texture on disk
		* @return The texture either from memory or the disk if found,
		* NO_TEXTURE otherwise
		*/
		Texture* getTexture(const std::string& fileName);
		/*
		* Get a sound from memory or the disk
		* 
		* @param fileName: The file name of this sound on disk
		* @return The sound either from memory or the disk if found,
		* NO_SOUND otherwise
		*/
		Sound* getSound(const std::string& fileName);
		/*
		* Delete all resources loaded into memory by this resource manager
		*/
		void destroy();

	private:
		// The map of textures in memory with their file names on disk
		std::unordered_map<std::string, Texture> m_textures;
		// The map of sounds in memory with their file names on disk
		std::unordered_map<std::string, Sound> m_sounds;

		/*
		* Convert a char* buffer to little-endian integer
		*/
		std::int32_t toInt(char* buffer, std::size_t len);
		/*
		* Load the header information of a WAVE audio file
		*/
		bool loadWAVHeader(std::ifstream& file, std::uint8_t& channels,
			std::int32_t& sampleRate, std::uint8_t& bitsPerSample,
			ALsizei& size);
		/*
		* Load a WAVE audio file
		*/
		char* loadWAV(const std::string& fileName, std::uint8_t& channels,
			std::int32_t& sampleRate, std::uint8_t& bitsPerSample,
			ALsizei& size);
	};
}