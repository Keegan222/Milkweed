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
#include <exception>

extern int decodePNG(std::vector<unsigned char>& out_image,
	unsigned long& image_width, unsigned long& image_height,
	const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);

namespace MW {
	/*
	* A general error for the resource manager to throw
	*/
	class ResourceException : public std::exception {
	public:
		ResourceException(const std::string& message) : m_message(message) {}
		virtual const char* what() const throw() {
			return m_message.c_str();
		}

	private:
		std::string m_message;
	};

	/*
	* A wrapper for the ID of an OpenGL texture
	*/
	class Texture {
	public:
		// The OpenGL ID of this texture
		GLuint textureID = 0;
		// The width of this texture in pixels
		unsigned int width = 0;
		// The height of this texture in pixels
		unsigned int height = 0;
	};

	/*
	* A wrapper for an OpenAL buffer ID
	*/
	struct Sound {
		// The ID number of this sound in OpenAL
		ALuint soundID = 0;
	};

	/*
	* A single character in a font
	*/
	struct Character {
		// The size of the character
		glm::vec2 dimensions = glm::vec2();
		// The offset from the origin of this character to the top-left
		glm::ivec2 bearing = glm::ivec2();
		// The distance to the next character in a string of text
		unsigned int offset = 0;
		// The texture of this character
		Texture texture;
	};

	/*
	* A wrapper for a map of char to Characters
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
		// An empty texture to return when texture loading fails
		static Texture NO_TEXTURE;
		// An empty sound to return when sound loading fails
		static Sound NO_SOUND;
		// An empty font to return when font loading fails
		static Font NO_FONT;

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
		Texture& getTexture(const std::string& fileName);
		/*
		* Get a sound from memory or the disk
		*
		* @param fileName: The file name of this sound on disk
		* @return The sound either from memory or the disk if found,
		* NO_SOUND otherwise
		*/
		Sound& getSound(const std::string& fileName);
		/*
		* Get a font from memory or the disk
		* 
		* @param fileName: The file name of this font on disk
		* @return The font either from memory or the disk if found,
		* NO_FONT otherwise
		*/
		Font& getFont(const std::string& fileName);
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
		// The instance of the freetype library to load fonts with
		FT_Library m_FTLib;
		// The map of fonts in memory with their file names on disk
		std::unordered_map<std::string, Font> m_fonts;
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