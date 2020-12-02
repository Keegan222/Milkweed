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
	* The Milkweed engine's utility for loading resources (textures and sound
	* effects) into the application
	*/
	class ResourceManager {
	public:
		/*
		* Get a texture from memory or the disk
		* 
		* @param fileName: The file name of the texture on disk
		* @return The texture either from memory or the disk if found, nullptr
		* otherwise
		*/
		Texture* getTexture(const std::string& fileName);
		/*
		* Delete all resources loaded into memory by this resource manager
		*/
		void destroy();

	private:
		// The map of textures in memory with their file names on disk
		std::unordered_map<std::string, Texture> m_textures;
	};
}