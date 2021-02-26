/*
* File: ResourceManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.22
*/

#ifndef MW_RESOURCES_H
#define MW_RESOURCES_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <AL/al.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "Sprite.h"

/*
* The decodePNG function is found in picoPNG.cpp, all documentation for it and
* copyright information can be found at the top of the picoPNG.cpp file or in
* the Deps/license/picoPNG/ directory of this project
*/
extern int decodePNG(std::vector<unsigned char>& out_image,
	unsigned long& image_width, unsigned long& image_height,
	const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);

namespace Milkweed {
	/*
	* A wrapper for the ID of an OpenGL texture
	*/
	struct Texture {
		// The OpenGL ID of this texture
		GLuint textureID = 0;
		// The dimensions of this texture in pixels
		glm::ivec2 dimensions = glm::ivec2();

		/*
		* Make a blank texture with no ID, width, or height
		*/
		Texture() : textureID(0), dimensions(glm::ivec2()) {}
		/*
		* Make a texture with the given ID, width, and height
		* 
		* @param textureID: The ID number of this texture in OpenGL
		* @param dimensions: The dimensions of this texture in pixels
		*/
		Texture(GLuint TextureID, const glm::ivec2& Dimensions) :
			textureID(TextureID), dimensions(Dimensions) {}
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
		Sound() : soundID(0) {}
		/*
		* Make a sound with the given ID
		* 
		* @param soundID: The ID number of this sound in OpenAL
		*/
		Sound(ALuint SoundID) : soundID(SoundID) {}
	};

	/*
	* A single character in a font
	*/
	struct Character {
		// The dimensions of this character
		glm::vec2 dimensions = glm::vec2();
		// The offset from the origin to this sprite to the top-left
		glm::ivec2 bearing = glm::ivec2();
		// The distance to the next character in a string of text
		unsigned int offset = 0;
		// The texture of this character
		Texture texture;

		/*
		* Make a blank character with no data
		*/
		Character() : dimensions(glm::vec2()), bearing(glm::ivec2()),
			offset(0), texture() {}
		/*
		* Make a character with the given dimensions, bearing, offset and
		* texture
		*/
		Character(const glm::vec2& Dimensions, const glm::ivec2& Bearing,
			unsigned int Offset, const Texture& cTexture)
			: dimensions(Dimensions), bearing(Bearing), offset(Offset),
			texture(cTexture) {}
	};

	/*
	* A font, a wrapper for a map of char's to Characters
	*/
	struct Font {
		std::map<char, Character> characters;
	};

	/*
	* The Milkweed framework's utility for loading resources (textures, sound
	* effects and fonts) into the application
	*/
	class ResourceManager {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		ResourceManager(ResourceManager& rm) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static ResourceManager& getInstance() {
			return m_instance;
		}

		/*
		* Prepare this resource manager to load textures, sounds, and fonts
		* 
		* @return Whether the resource manager was initialized successfully
		*/
		bool init();
		/*
		* Get a PNG texture from memory or the disk
		*
		* @param fileName: The file name of the texture on disk
		* @return The texture either from memory or the disk if found, nullptr
		* otherwise
		*/
		Texture* getTexture(const std::string& fileName);
		/*
		* Get a WAV sound from memory or the disk
		*
		* @param fileName: The file name of this sound on disk
		* @return The sound either from memory or the disk if found, nullptr
		* otherwise
		*/
		Sound* getSound(const std::string& fileName);
		/*
		* Get a font from memory or the disk
		*
		* @param fileName: The file name of this TTF font on disk
		* @return The font either from memory or the disk if found, nullptr
		* otherwise
		*/
		Font* getFont(const std::string& fileName);
		/*
		* Test whether this resource manager can load TTF files
		*/
		bool isFontLoadingEnabled() const { return m_fontLoadingEnabled; }
		/*
		* Get the point size this resource manager loads TTF fonts at
		*/
		unsigned int getFontPointSize() const { return m_fontPointSize; }
		/*
		* Set the point size to load fonts from TTF files at
		*/
		void setFontPointSize(unsigned int fontPointSize) {
			m_fontPointSize = fontPointSize;
		}
		/*
		* Delete all resources loaded into memory by this resource manager
		*/
		void destroy();

	private:
		// The singleton instance of this class
		static ResourceManager m_instance;
		/*
		* The constructor is disabled for this class
		*/
		ResourceManager() {}

		// The map of textures in memory with their file names on disk
		std::unordered_map<std::string, Texture> m_textures;
		// The map of sounds in memory with their file names on disk
		std::unordered_map<std::string, Sound> m_sounds;
		// The map of fonts in memory with their file names on disk
		std::unordered_map<std::string, Font> m_fonts;
		// The instance of the FreeType library to load fonts with
		FT_Library m_freeTypeLibrary = nullptr;
		// Whether this resource manager can load TTF files
		bool m_fontLoadingEnabled = false;
		// The default point size of fonts
		FT_UInt m_fontPointSize = 48;

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

#endif