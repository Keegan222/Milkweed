/*
* File: AudioManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.12.16
*/

#include "MW.h"

namespace Milkweed {
	AudioManager AudioManager::m_instance;

	bool AudioManager::init() {
		MWLOG(Info, AudioManager, "Initializing OpenAL-Soft audio");

		// Open the default sound device with OpenAL
		m_device = alcOpenDevice(nullptr);
		if (m_device == nullptr) {
			MWLOG(Error, AudioManager, "Failed to open default audio device");
			return false;
		}

		// Create an OpenAL context and make it current
		m_context = alcCreateContext(m_device, nullptr);
		if (m_context == nullptr) {
			MWLOG(Error, AudioManager, "Failed to create OpenAL context");
			return false;
		}
		ALCboolean contextCurrent = alcMakeContextCurrent(m_context);
		if (contextCurrent != ALC_TRUE) {
			MWLOG(Error, AudioManager, "Failed to make OpenAL context current");
			return false;
		}

		MWLOG(Info, AudioManager, "Opened OpenAL audio device \"",
			alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER), "\"");

		// Set up the music audio source
		m_musicSourceID = createSource(true);

		// Set up 10 initial sources for sound effects (more can be added later
		// in playSound()
		m_effectSources.resize(10);
		for (unsigned int i = 0; i < 10; i++) {
			ALuint source = createSource();
			m_effectSources[i] = source;
		}

		return true;
	}

	void AudioManager::setGain(float gain) {
		// Clamp the gain between 0.0f and 1.0f
		if (gain < 0.0f) {
			gain = 0.0f;
		}
		else if (gain > 1.0f) {
			gain = 1.0f;
		}

		// Set the gain for tracking
		m_gain = gain;

		alSourcef(m_musicSourceID, AL_GAIN, m_gain);
		for (ALuint sourceID : m_effectSources) {
			alSourcef(sourceID, AL_GAIN, m_gain);
		}
	}

	void AudioManager::playMusic(const Sound* music) {
		stopMusic();
		if (music == nullptr) {
			return;
		}
		alSourcei(m_musicSourceID, AL_BUFFER, music->soundID);
		playMusic();
	}

	void AudioManager::playMusic() {
		alSourcePlay(m_musicSourceID);
	}

	void AudioManager::pauseMusic() {
		alSourcePause(m_musicSourceID);
	}

	void AudioManager::stopMusic() {
		alSourceStop(m_musicSourceID);
		alSourcei(m_musicSourceID, AL_BUFFER, 0);
	}

	void AudioManager::playSound(const Sound* sound) {
		// Find a sound effect source which is not currently playing or paused
		ALuint source = 0;
		for (unsigned int i = 0; i < m_effectSources.size(); i++) {
			ALint state = getSourceState(m_effectSources[i]);
			if (state == AL_INITIAL || state == AL_STOPPED) {
				source = m_effectSources[i];
				break;
			}
		}

		if (source == 0) {
			// No source was found, a new one must be generated and added
			source = createSource();
			m_effectSources.push_back(source);
		}

		// Bind the buffer to the source and play it
		alSourcei(source, AL_BUFFER, sound->soundID);
		alSourcePlay(source);
	}

	void AudioManager::stop() {
		// Stop the music track
		stopMusic();

		// Stop all sound effects
		for (ALuint sourceID : m_effectSources) {
			alSourceStop(sourceID);
			alSourcei(sourceID, AL_BUFFER, 0);
		}
	}

	void AudioManager::destroy() {
		int count = 1;
		// Delete the music and sound effect sources
		alDeleteSources(1, &m_musicSourceID);
		for (unsigned int i = 0; i < m_effectSources.size(); i++) {
			alDeleteSources(1, &m_effectSources[i]);
			count++;
		}
		m_effectSources.clear();

		MWLOG(Info, AudioManager, "Stopping audio system, deleted ", count,
			" audio sources from OpenAL");

		// Clear the OpenAL context and close the audio device
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);

		MWLOG(Info, ResourceManager, "Destroyed OpenAL context and closed ",
			"audio device");
	}

	ALuint AudioManager::createSource(bool looping) {
		ALuint sourceID;
		alGenSources(1, &sourceID);
		alSourcef(sourceID, AL_PITCH, 1.0f);
		alSourcef(sourceID, AL_GAIN, m_gain);
		alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(sourceID, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		if (looping) {
			alSourcei(sourceID, AL_LOOPING, AL_TRUE);
		}
		else {
			alSourcei(sourceID, AL_LOOPING, AL_FALSE);
		}
		return sourceID;
	}

	ALint AudioManager::getSourceState(ALuint source) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		return state;
	}
}