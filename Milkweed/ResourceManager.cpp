/*
* File: ResourceManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.27.1438
*/

#include <fstream>

#include "MW.h"
#include "ResourceManager.h"

namespace MW {
	Texture ResourceManager::NO_TEXTURE = Texture();

	Texture* ResourceManager::getTexture(const std::string& fileName) {
		std::unordered_map<std::string, Texture>::iterator it
			= m_textures.find(fileName);
		if (it != m_textures.end()) {
			// The texture is already present in memory, return it
			return &m_textures[fileName];
		}

		// The texture is not present in memory and must be loaded
		std::ifstream textureFile(fileName.c_str(), std::ios::in
			| std::ios::binary | std::ios::ate);
		std::streamsize fileSize = 0;
		if (textureFile.seekg(0, std::ios::end).good()) {
			// Get the size of the file in bytes
			fileSize = textureFile.tellg();
		}
		if (textureFile.seekg(0, std::ios::beg).good()) {
			// Remove the size of the file's header data
			fileSize -= textureFile.tellg();
		}
		// Read in the file's data
		std::vector<unsigned char> buffer;
		if (fileSize > 0) {
			buffer.resize((size_t)fileSize);
			textureFile.read((char*)(&buffer[0]), fileSize);
		}
		else {
			// The file could not be read
			App::Log("Failed to read texture data file");
			return &ResourceManager::NO_TEXTURE;
		}

		// Decode the texture file's data with picoPNG's decodePNG function
		std::vector<unsigned char> textureData;
		unsigned long textureWidth = 0, textureHeight = 0;
		int status = decodePNG(textureData, textureWidth, textureHeight,
			&buffer[0], (size_t)fileSize);
		if (status != 0) {
			// The texture could not be decoded in PNG format
			App::Log("Failed to decode PNG texture with code "
				+ std::to_string(status));
			return &ResourceManager::NO_TEXTURE;
		}

		// Create this texture and upload its data to OpenGL
		GLuint textureID = 0;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Add this texture to the map of textures in memory
		m_textures[fileName] = Texture(textureID, (unsigned int)textureWidth,
			(unsigned int)textureHeight);
		return &m_textures[fileName];
	}

	void ResourceManager::destroy() {
		for (std::pair<std::string, Texture> pair : m_textures) {
			App::Log("Delete texture " + std::to_string(pair.second.textureID));
			Texture texture = pair.second;
			glDeleteTextures(1, &texture.textureID);
		}
	}
}