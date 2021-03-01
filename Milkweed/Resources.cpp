/*
* File: ResourceManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.11.27
*/

#include <fstream>

#include "MW.h"

namespace Milkweed {
	ResourceManager ResourceManager::m_instance;

	void ResourceManager::init() {
		// Initialize freetype
		if (FT_Init_FreeType(&m_freeTypeLibrary) != FT_Err_Ok) {
			m_fontLoadingEnabled = false;
			MWLOG(Warning, ResourceManager, "Failed to initialize freetype2 ",
				"library, font loading will be disabled");
			return;
		}

		m_fontLoadingEnabled = true;
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
		if (textureFile.fail()) {
			MWLOG(Warning, ResourceManager, "Failed to load texture file ",
				fileName);
			return nullptr;
		}
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
			MWLOG(Warning, ResourceManager, "Failed to load texture file ",
				fileName);
			return nullptr;
		}

		// Decode the texture file's data with picoPNG's decodePNG function
		std::vector<unsigned char> textureData;
		unsigned long textureWidth = 0, textureHeight = 0;
		int status = decodePNG(textureData, textureWidth, textureHeight,
			&buffer[0], (size_t)fileSize);
		if (status != 0) {
			// The texture could not be decoded in PNG format
			MWLOG(Warning, ResourceManager, "Failed to decode PNG file ",
				fileName, ", may be in invalid format");
			return nullptr;
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
		m_textures[fileName] = Texture(textureID,
			glm::ivec2((unsigned int)textureWidth,
			(unsigned int)textureHeight));
		return &m_textures[fileName];
	}

	Sound* ResourceManager::getSound(const std::string& fileName) {
		// Attempt to find the sound in memory
		std::unordered_map<std::string, Sound>::iterator it
			= m_sounds.find(fileName);
		if (it != m_sounds.end()) {
			// The sound was found in memory, return it
			return &m_sounds[fileName];
		}

		// The sound was not found in memory and must be loaded from the disk
		// Get the header information and the sound data
		std::uint8_t channels;
		std::int32_t sampleRate;
		std::uint8_t bitsPerSample;
		ALsizei size;
		char* soundData = loadWAV(fileName, channels, sampleRate,
			bitsPerSample, size);
		if (soundData == nullptr) {
			MWLOG(Warning, ResourceManager, "Failed to load audio file ",
				fileName);
			return nullptr;
		}

		// Derive the sound's OpenAL format from the header info
		ALenum format;
		if (channels == 1 && bitsPerSample == 8) {
			format = AL_FORMAT_MONO8;
		}
		else if (channels == 1 && bitsPerSample == 16) {
			format = AL_FORMAT_MONO16;
		}
		else if (channels == 2 && bitsPerSample == 8) {
			format = AL_FORMAT_STEREO8;
		}
		else if (channels == 2 && bitsPerSample == 16) {
			format = AL_FORMAT_STEREO16;
		}
		else {
			MWLOG(Warning, ResourceManager, "Audio file ", fileName,
				" is in invalid format for OpenAL");
			return nullptr;
		}

		// Create the sound buffer and upload the sound data to it, then remove
		// the sound data from RAM
		Sound sound;
		alGenBuffers(1, &sound.soundID);
		alBufferData(sound.soundID, format, soundData, size, sampleRate);
		delete soundData;

		// Place the new sound into the map and return it
		m_sounds[fileName] = sound;
		return &m_sounds[fileName];
	}

	Font* ResourceManager::getFont(const std::string& fileName) {
		if (!m_fontLoadingEnabled) {
			// If font loading is disabled because FT could not be initialized,
			// do not attempt to load this font
			MWLOG(Warning, ResourceManager, "Failed to load font ", fileName,
				" because font loading is disabled");
			return nullptr;
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
			MWLOG(Warning, ResourceManager, "Failed to read font ", fileName);
			return nullptr;
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
				// This character is not in the font
				MWLOG(Warning, ResourceManager, "Failed to load character ", c,
					" from font ", fileName);
				continue;
			}
			// Allocate a new texture for this character and upload FreeType
			// data to it in OpenGL
			Texture texture;
			glGenTextures(1, &texture.textureID);
			glBindTexture(GL_TEXTURE_2D, texture.textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
				face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);
			texture.dimensions.x = face->glyph->bitmap.width;
			texture.dimensions.y = face->glyph->bitmap.rows;
			// Add the character to the font's character map
			font.characters[c] = Character(glm::vec2(texture.dimensions.x,
				texture.dimensions.y), glm::ivec2(face->glyph->bitmap_left,
					face->glyph->bitmap_top), face->glyph->advance.x >> 6,
				texture);
		}

		FT_Done_Face(face);
		m_fonts[fileName] = font;
		return &m_fonts[fileName];
	}

	void ResourceManager::destroy() {
		MWLOG(Info, ResourceManager, "Destroying resources loading from disk");

		int count = 0;
		// Delete all of the textures loaded into memory from OpenGL
		for (std::pair<std::string, Texture> pair : m_textures) {
			glDeleteTextures(1, &pair.second.textureID);
			count++;
		}
		m_textures.clear();

		MWLOG(Info, ResourceManager, "Deleted ", count, " textures from ",
			"OpenGL");

		count = 0;
		// Delete all the sounds loaded into memory from OpenAL
		for (std::pair<std::string, Sound> pair : m_sounds) {
			alDeleteBuffers(1, &pair.second.soundID);
			count++;
		}
		m_sounds.clear();

		MWLOG(Info, ResourceManager, "Deleted ", count, " sound buffers from ",
			"OpenAL");

		if (!m_fontLoadingEnabled) {
			MWLOG(Info, ResourceManager, "No fonts to delete");
			return;
		}
		count = 0;
		// Delete all fonts loaded into memory and dispose of the FreeType lib
		for (const std::pair<std::string, Font>& pair : m_fonts) {
			for (const std::pair<char, Character>& c : pair.second.characters) {
				glDeleteTextures(1, &c.second.texture.textureID);
			}
			count++;
		}
		m_fonts.clear();
		FT_Done_FreeType(m_freeTypeLibrary);

		MWLOG(Info, ResourceManager, "Deleted ", count, " font character sets ",
			"from OpenGL");
	}

	std::int32_t ResourceManager::toInt(char* buffer, std::size_t len) {
		std::int32_t a = 0;
		std::memcpy(&a, buffer, len);
		return a;
	}

	bool ResourceManager::loadWAVHeader(std::ifstream& file,
		std::uint8_t& channels, std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample, ALsizei& size) {
		char buffer[4];
		if (!file.is_open()) {
			MWLOG(Warning, ResourceManager, "Failed to open WAV file header");
			return false;
		}

		// Check the "RIFF" header (chunkID)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read RIFF");
			return false;
		}
		if (std::strncmp(buffer, "RIFF", 4) != 0) {
			MWLOG(Warning, ResourceManager, "File is in invalid WAVE format");
			return false;
		}

		// Discard the size of the file (chunkSize)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read file size");
			return false;
		}

		// Check the "WAVE" header (format)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read WAVE");
			return false;
		}
		if (std::strncmp(buffer, "WAVE", 4) != 0) {
			MWLOG(Warning, ResourceManager, "File is in invalid WAVE format");
			return false;
		}

		// Check the "fmt" header (subchunk1ID)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read fmt");
			return false;
		}
		if (std::strncmp(buffer, "fmt", 3) != 0) {
			MWLOG(Warning, ResourceManager, "File is in invalid WAVE format");
			return false;
		}

		// Discard the size of the format chunk (subchunk1Size)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read format chunk size");
			return false;
		}

		// Discard the audio format which should be 1 for PCM (audioFormat)
		if (!file.read(buffer, 2)) {
			MWLOG(Warning, ResourceManager, "Failed to read audio format");
			return false;
		}

		// Get the number of channels (numChannels)
		if (!file.read(buffer, 2)) {
			MWLOG(Warning, ResourceManager, "Failed to read channel count");
			return false;
		}
		channels = toInt(buffer, 2);

		// Get the sample rate (sampleRate)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read the sample rate");
			return false;
		}
		sampleRate = toInt(buffer, 4);

		// Discard the byte rate (byteRate)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read the byte rate");
			return false;
		}

		// Discard the block alignment (blockAlign)
		if (!file.read(buffer, 2)) {
			MWLOG(Warning, ResourceManager, "Failed to read the block ",
				"alignment");
			return false;
		}

		// Get the bits in each sample (bitsPerSample)
		if (!file.read(buffer, 2)) {
			MWLOG(Warning, ResourceManager, "Failed to read the bit count of ",
				"each audio sample");
			return false;
		}
		bitsPerSample = toInt(buffer, 2);

		// Check the "data" header (subchunk2ID)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read the data header");
			return false;
		}
		if (std::strncmp(buffer, "data", 4) != 0) {
			MWLOG(Warning, ResourceManager, "File is in invalid WAVE format");
			return false;
		}

		// Get the size of the audio data (subchunk2Size)
		if (!file.read(buffer, 4)) {
			MWLOG(Warning, ResourceManager, "Failed to read the size of the ",
				"audio data");
			return false;
		}
		size = toInt(buffer, 4);

		if (file.eof()) {
			MWLOG(Warning, ResourceManager, "File contains no audio data");
			return false;
		}
		if (file.fail()) {
			//MW::LOG << "File could not be read further\n";
			MWLOG(Warning, ResourceManager, "Failed to read audio file past ",
				"header data");
			return false;
		}

		return true;
	}

	char* ResourceManager::loadWAV(const std::string& fileName,
		std::uint8_t& channels, std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample, ALsizei& size) {
		std::ifstream in(fileName, std::ios::binary);
		if (!in.is_open()) {
			MWLOG(Warning, ResourceManager, "Failed to open file ", fileName);
			return nullptr;
		}
		if (!loadWAVHeader(in, channels, sampleRate, bitsPerSample, size)) {
			MWLOG(Warning, ResourceManager, "Failed to read file header for ",
				fileName);
			return nullptr;
		}

		char* data = new char[size];
		in.read(data, size);
		in.close();
		return data;
	}
}