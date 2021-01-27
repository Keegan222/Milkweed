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
#include <map>
#include <AL/al.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "Sprite.h"

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
	* A single character in a font
	*/
	class Character : public Sprite {
	public:
		// The offset from the origin to this sprite to the top-left
		glm::ivec2 bearing = glm::ivec2();
		// The distance to the next character in a string of text
		unsigned int offset = 0;

		/*
		* Construct a blank character
		*/
		Character() {}
		/*
		* Create a new character with dimensions, bearing, offset, and texture
		*/
		Character(const glm::vec2& dimensions, const glm::ivec2& Bearing,
			unsigned int Offset, Texture* texture) : Sprite(glm::vec3(0.0f),
				dimensions, texture), bearing(Bearing), offset(Offset) {}
		/*
		* Update this character (does nothing)
		*/
		void update(float deltaTime) override {};
	};

	/*
	* A font, wrapper for a map of char to Characters
	*/
	struct Font {
		std::map<char, Character> characters;
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
		// An empty font to initialize objects with
		static Font* NO_FONT;

		// Disable the copy constructor
		ResourceManager(ResourceManager& rm) = delete;
		/*
		* Prepare this resource manager to load textures, sounds, and fonts
		*/
		bool init();
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
		* Get a font from memory or the disk
		*
		* @param fileName: The file name of this TTF font on disk
		* @return The font either from memory or the disk if found,
		* NO_FONT otherwise
		*/
		Font* getFont(const std::string& fileName);
		/*
		* Set the point size to load fonts from TTF files at
		*
		* @param fontPointSize: The pixel height to load fonts at from TTF
		* files, larger sizes take more video memory
		*/
		void setFontPointSize(unsigned int fontPointSize) {
			m_fontPointSize = fontPointSize;
		}
		/*
		* Test whether this resource manager can load TTF files
		*/
		bool isFontLoadingEnabled() const { return m_fontLoadingEnabled; }
		/*
		* Delete all resources loaded into memory by this resource manager
		*/
		void destroy();
		/*
		* Get the singleton instance of this class
		*/
		static ResourceManager& getInstance() {
			return m_instance;
		}

	private:
		// The map of textures in memory with their file names on disk
		std::unordered_map<std::string, Texture> m_textures;
		// The map of sounds in memory with their file names on disk
		std::unordered_map<std::string, Sound> m_sounds;
		// The map of fonts in memory with their file names on disk
		std::unordered_map<std::string, Font> m_fonts;
		// The instance of the FreeType library to load fonts with
		FT_Library m_freeTypeLibrary;
		// Whether this resource manager can load TTF files
		bool m_fontLoadingEnabled = false;
		// The default point size of fonts
		FT_UInt m_fontPointSize = 48;
		// The singleton instance of this class
		static ResourceManager m_instance;

		// Disable the constructor
		ResourceManager() {}
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