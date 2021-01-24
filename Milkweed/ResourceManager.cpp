/*
* File: ResourceManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.27.1438
*/

#include <fstream>
#include <iostream>

#include "MW.h"
#include "ResourceManager.h"

namespace MW {
	Texture* ResourceManager::NO_TEXTURE = new Texture();
	Font* ResourceManager::NO_FONT = new Font();
	ResourceManager ResourceManager::m_instance;

	bool ResourceManager::init() {
		// Initialize freetype
		if (FT_Init_FreeType(&m_freeTypeLibrary) != FT_Err_Ok) {
			m_fontLoadingEnabled = false;
			return false;
		}

		m_fontLoadingEnabled = true;
		return true;
	}

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
			App::LOG << "Failed to read texture data file\n";
			return NO_TEXTURE;
		}

		// Decode the texture file's data with picoPNG's decodePNG function
		std::vector<unsigned char> textureData;
		unsigned long textureWidth = 0, textureHeight = 0;
		int status = decodePNG(textureData, textureWidth, textureHeight,
			&buffer[0], (size_t)fileSize);
		if (status != 0) {
			// The texture could not be decoded in PNG format
			App::LOG << "Failed to decode PNG texture with code " << status
				<< "\n";
			return NO_TEXTURE;
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

	Font* ResourceManager::getFont(const std::string& fileName) {
		if (!m_fontLoadingEnabled) {
			// If font loading is disabled because FT could not be initialized,
			// do not attempt to load this font
			return NO_FONT;
		}

		std::unordered_map<std::string, Font>::iterator it
			= m_fonts.find(fileName);
		if (it != m_fonts.end()) {
			// The font was found in memory, return it
			return &m_fonts[fileName];
		}

		// The font was not found in memory and must be loaded from the disk
		FT_Face face;
		if (FT_New_Face(m_freeTypeLibrary, fileName.c_str(), 0, &face)
			!= FT_Err_Ok) {
			// The font could not be loaded from disk
			return NO_FONT;
		}
		// Set the point size to load the font at
		FT_Set_Pixel_Sizes(face, 0, m_fontPointSize);

		// Create a new font object
		Font font;
		// Iterate over the first 128 characters
		for (unsigned char c = 0; c < 128; c++) {
			// Load the character
			FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
			if (error != FT_Err_Ok) {
				App::LOG << "Failed to load character " << c << " with code "
					<< error << "\n";
				// This character is not in the font
				continue;
			}
			// Allocate a new texture for this character and upload FreeType
			// data to it in OpenGL
			Texture* texture = new Texture();
			glGenTextures(1, &texture->textureID);
			glBindTexture(GL_TEXTURE_2D, texture->textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
				face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);
			texture->width = face->glyph->bitmap.width;
			texture->height = face->glyph->bitmap.rows;
			// Add the texture to a new character
			Character character = Character(
				glm::vec2(texture->width, texture->height),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x, texture);
			// Add the character to the font's character map
			font.characters.insert(std::pair<char, Character>(c, character));
		}

		FT_Done_Face(face);
		m_fonts[fileName] = font;
		return &m_fonts[fileName];
	}

	void ResourceManager::destroy() {
		// Delete all of the textures loaded into memory from OpenGL
		for (std::pair<std::string, Texture> pair : m_textures) {
			App::LOG << "Delete texture " << pair.second.textureID << "\n";
			glDeleteTextures(1, &pair.second.textureID);
		}
		m_textures.clear();

		// Delete all fonts loaded into memory and dispose of the FreeType lib
		for (std::pair<std::string, Font> pair : m_fonts) {
			App::LOG << "Delete font " << pair.first << "\n";
			for (std::pair<char, Character> c : pair.second.characters) {
				glDeleteTextures(1, &c.second.texture->textureID);
				delete c.second.texture;
			}
		}
		FT_Done_FreeType(m_freeTypeLibrary);
	}
}