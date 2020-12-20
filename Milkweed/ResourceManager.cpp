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
	Sound* ResourceManager::NO_SOUND = new Sound();

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
			return NO_TEXTURE;
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

	Sound* ResourceManager::getSound(const std::string& fileName) {
		// Attempt to find the sound in memory
		std::unordered_map<std::string, Sound>::iterator it
			= m_sounds.find(fileName);
		if (it != m_sounds.end()) {
			App::Log("Found " + fileName + " in memory");
			// The sound was found in memory, return it
			return &m_sounds[fileName];
		}

		App::Log("Could not find " + fileName + " in memory");

		// The sound was not found in memory and must be loaded from the disk
		// Get the header information and the sound data
		std::uint8_t channels;
		std::int32_t sampleRate;
		std::uint8_t bitsPerSample;
		ALsizei size;
		char* soundData = loadWAV(fileName, channels, sampleRate,
			bitsPerSample, size);
		if (soundData == nullptr) {
			App::Log("Failed to load sound " + fileName);
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
			App::Log("Sound " + fileName + " is in invalid format for OpenAL");
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
		return &m_sounds[fileName];
	}

	void ResourceManager::destroy() {
		// Delete all of the textures loaded into memory from OpenGL
		for (std::pair<std::string, Texture> pair : m_textures) {
			App::Log("Delete texture " + std::to_string(
				pair.second.textureID));
			glDeleteTextures(1, &pair.second.textureID);
		}
		m_textures.clear();

		// Delete all the sounds loaded into memory from OpenAL
		for (std::pair<std::string, Sound> pair : m_sounds) {
			App::Log("Delete sound " + std::to_string(
				pair.second.soundID));
			alDeleteBuffers(1, &pair.second.soundID);
		}
		m_sounds.clear();
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
			App::Log("Failed to open WAV file header");
			return false;
		}

		// Check the "RIFF" header (chunkID)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read RIFF");
			return false;
		}
		if (std::strncmp(buffer, "RIFF", 4) != 0) {
			App::Log("File is not in valid WAVE format");
			return false;
		}

		// Discard the size of the file (chunkSize)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read file size");
			return false;
		}

		// Check the "WAVE" header (format)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read WAVE");
			return false;
		}
		if (std::strncmp(buffer, "WAVE", 4) != 0) {
			App::Log("File is not in valid WAVE format");
			return false;
		}

		// Check the "fmt" header (subchunk1ID)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read fmt");
			return false;
		}
		if (std::strncmp(buffer, "fmt", 3) != 0) {
			App::Log("File is not in valid WAVE format");
			return false;
		}

		// Discard the size of the format chunk (subchunk1Size)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read format chunk size");
			return false;
		}

		// Discard the audio format which should be 1 for PCM (audioFormat)
		if (!file.read(buffer, 2)) {
			App::Log("Failed to read audio format");
			return false;
		}

		// Get the number of channels (numChannels)
		if (!file.read(buffer, 2)) {
			App::Log("Failed to read channel count");
			return false;
		}
		channels = toInt(buffer, 2);
		
		// Get the sample rate (sampleRate)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read the sample rate");
			return false;
		}
		sampleRate = toInt(buffer, 4);

		// Discard the byte rate (byteRate)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read the byte rate");
			return false;
		}

		// Discard the block alignment (blockAlign)
		if (!file.read(buffer, 2)) {
			App::Log("Failed to read the block alignment");
			return false;
		}

		// Get the bits in each sample (bitsPerSample)
		if (!file.read(buffer, 2)) {
			App::Log("Failed to read the bits in each sample");
			return false;
		}
		bitsPerSample = toInt(buffer, 2);

		// Check the "data" header (subchunk2ID)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read the data header");
			return false;
		}
		if (std::strncmp(buffer, "data", 4) != 0) {
			App::Log("File is in invalid WAVE format");
			return false;
		}

		// Get the size of the audio data (subchunk2Size)
		if (!file.read(buffer, 4)) {
			App::Log("Failed to read the size of the audio data");
			return false;
		}
		size = toInt(buffer, 4);

		if (file.eof()) {
			App::Log("File contains no audio data");
			return false;
		}
		if (file.fail()) {
			App::Log("File could not be read further");
			return false;
		}

		return true;
	}

	char* ResourceManager::loadWAV(const std::string& fileName,
		std::uint8_t& channels, std::int32_t& sampleRate,
		std::uint8_t& bitsPerSample, ALsizei& size) {
		std::ifstream in(fileName, std::ios::binary);
		if (!in.is_open()) {
			App::Log("Failed to open file " + fileName);
			return nullptr;
		}
		if (!loadWAVHeader(in, channels, sampleRate, bitsPerSample, size)) {
			App::Log("Failed to read file header for " + fileName);
			return nullptr;
		}

		char* data = new char[size];
		in.read(data, size);
		in.close();
		return data;
	}
}