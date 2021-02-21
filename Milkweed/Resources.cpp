/*
* File: ResourceManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.11.27.1438
*/

#include <fstream>
#include <iostream>

#include "MW.h"
#include "Resources.h"

namespace MW {
	Texture ResourceManager::NO_TEXTURE;
	Sound ResourceManager::NO_SOUND;
	ResourceManager ResourceManager::m_instance;

	bool ResourceManager::init() {
		// Initialize the freetype library
		if (FT_Init_FreeType(&m_FTLib) != FT_Err_Ok) {
			App::LOG << "Failed to initialize FreeType2 for font loading\n";
			return false;
		}
		return true;
	}

	Texture& ResourceManager::getTexture(const std::string& fileName) {
		std::unordered_map<std::string, Texture>::iterator it
			= m_textures.find(fileName);
		if (it != m_textures.end()) {
			// The texture is already present in memory, return it
			return m_textures[fileName];
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
			throw ResourceException("Failed to read texture data file");
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
			throw ResourceException("Failed to decode PNG texture");
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
		Texture texture;
		texture.textureID = textureID;
		texture.width = textureWidth;
		texture.height = textureHeight;
		m_textures[fileName] = texture;
		return m_textures[fileName];
	}

	Sound& ResourceManager::getSound(const std::string& fileName) {
		// Attempt to find the sound in memory
		std::unordered_map<std::string, Sound>::iterator it
			= m_sounds.find(fileName);
		if (it != m_sounds.end()) {
			// The sound was found in memory, return it
			return m_sounds[fileName];
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
			App::LOG << "Failed to load sound " << fileName << "\n";
			throw ResourceException("Failed to read sound file");
			return NO_SOUND;
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
			App::LOG << "Sound " << fileName
				<< " is in invalid format for OpenAL\n";
			throw ResourceException("Sound in invalid WAV format");
			return NO_SOUND;
		}

		// Create the sound buffer and upload the sound data to it, then remove
		// the sound data from RAM
		Sound sound;
		alGenBuffers(1, &sound.soundID);
		alBufferData(sound.soundID, format, soundData, size, sampleRate);
		delete soundData;

		// Place the new sound into the map and return it
		m_sounds[fileName] = sound;
		return m_sounds[fileName];
	}

	Font& ResourceManager::getFont(const std::string& fileName) {
		// Attempt to find the font in memory
		std::unordered_map<std::string, Font>::iterator it
			= m_fonts.find(fileName);
		if (it != m_fonts.end()) {
			// The font was found in memory, return it
			return m_fonts[fileName];
		}

		// The font was not found in memory and must be loaded from the disk
		FT_Face face;
		if (FT_New_Face(m_FTLib, fileName.c_str(), 0, &face)
			!= FT_Err_Ok) {
			throw ResourceException("Failed to load font " + fileName);
			return NO_FONT;
		}
		// Set the size of the font
		// TODO: Do not hardcode this to 48
		FT_Set_Pixel_Sizes(face, 0, 48);

		// Load the characters from the font
		Font font;
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER) != FT_Err_Ok) {
				throw ResourceException("Failed to load character "
					+ std::to_string(c) + " in font " + fileName);
				continue;
			}

			Character ch;
			// Create the texture for the character
			glGenTextures(1, &ch.texture.textureID);
			glBindTexture(GL_TEXTURE_2D, ch.texture.textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
				face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);
			ch.texture.width = face->glyph->bitmap.width;
			ch.texture.height = face->glyph->bitmap.rows;
			// Set the dimensions, bearing, and offset of the character
			ch.dimensions = glm::vec2(ch.texture.width, ch.texture.height);
			ch.bearing = glm::ivec2(face->glyph->bitmap_left,
				face->glyph->bitmap_top);
			ch.offset = face->glyph->advance.x;
			// Add the character to the font
			font.characters.insert(std::pair<char, Character>(c, ch));
		}

		// Dispose of the face in freetype and place the font in memory
		FT_Done_Face(face);
		m_fonts[fileName] = font;
		return m_fonts[fileName];
	}

	void ResourceManager::destroy() {
		// Delete all of the textures loaded into memory from OpenGL
		for (std::pair<std::string, Texture> pair : m_textures) {
			App::LOG << "Delete texture " << pair.second.textureID << "\n";
			glDeleteTextures(1, &pair.second.textureID);
		}
		m_textures.clear();

		// Make sure that the audio system has stopped before deleting its data
		App::AUDIO.stop();

		// Delete all the sounds loaded into memory from OpenAL
		for (std::pair<std::string, Sound> pair : m_sounds) {
			App::LOG << "Delete sound " << pair.second.soundID << "\n";
			alDeleteBuffers(1, &pair.second.soundID);
		}
		m_sounds.clear();

		// Delete all the fonts loaded into memory and dispose of the freetype
		// library
		m_fonts.clear();
		FT_Done_FreeType(m_FTLib);
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
			App::LOG << "Failed to open WAV file header\n";
			return false;
		}

		// Check the "RIFF" header (chunkID)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read RIFF\n";
			return false;
		}
		if (std::strncmp(buffer, "RIFF", 4) != 0) {
			App::LOG << "File is not in valid WAVE format\n";
			return false;
		}

		// Discard the size of the file (chunkSize)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read file size\n";
			return false;
		}

		// Check the "WAVE" header (format)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read WAVE\n";
			return false;
		}
		if (std::strncmp(buffer, "WAVE", 4) != 0) {
			App::LOG << "File is not in valid WAVE format\n";
			return false;
		}

		// Check the "fmt" header (subchunk1ID)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read fmt\n";
			return false;
		}
		if (std::strncmp(buffer, "fmt", 3) != 0) {
			App::LOG << "File is not in valid WAVE format\n";
			return false;
		}

		// Discard the size of the format chunk (subchunk1Size)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read format chunk size\n";
			return false;
		}

		// Discard the audio format which should be 1 for PCM (audioFormat)
		if (!file.read(buffer, 2)) {
			App::LOG << "Failed to read audio format\n";
			return false;
		}

		// Get the number of channels (numChannels)
		if (!file.read(buffer, 2)) {
			App::LOG << "Failed to read channel count\n";
			return false;
		}
		channels = toInt(buffer, 2);

		// Get the sample rate (sampleRate)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read the sample rate\n";
			return false;
		}
		sampleRate = toInt(buffer, 4);

		// Discard the byte rate (byteRate)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read the byte rate\n";
			return false;
		}

		// Discard the block alignment (blockAlign)
		if (!file.read(buffer, 2)) {
			App::LOG << "Failed to read the block alignment\n";
			return false;
		}

		// Get the bits in each sample (bitsPerSample)
		if (!file.read(buffer, 2)) {
			App::LOG << "Failed to read the bits in each sample\n";
			return false;
		}
		bitsPerSample = toInt(buffer, 2);

		// Check the "data" header (subchunk2ID)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read the data header\n";
			return false;
		}
		if (std::strncmp(buffer, "data", 4) != 0) {
			App::LOG << "File is in invalid WAVE format\n";
			return false;
		}

		// Get the size of the audio data (subchunk2Size)
		if (!file.read(buffer, 4)) {
			App::LOG << "Failed to read the size of the audio data\n";
			return false;
		}
		size = toInt(buffer, 4);

		if (file.eof()) {
			App::LOG << "File contains no audio data\n";
			return false;
		}
		if (file.fail()) {
			App::LOG << "File could not be read further\n";
			return false;
		}

		return true;
	}

	char* ResourceManager::loadWAV(const std::string& fileName,
		std::uint8_t& channels, std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample, ALsizei& size) {
		std::ifstream in(fileName, std::ios::binary);
		if (!in.is_open()) {
			App::LOG << "Failed to open file " << fileName << "\n";
			return nullptr;
		}
		if (!loadWAVHeader(in, channels, sampleRate, bitsPerSample, size)) {
			App::LOG << "Failed to read file header for " << fileName << "\n";
			return nullptr;
		}

		char* data = new char[size];
		in.read(data, size);
		in.close();
		return data;
	}
}