/*
* File: AudioManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.12.16.1146
*/

#ifndef MW_AUDIO_H
#define MW_AUDIO_H

#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include "Resources.h"

namespace Milkweed {
	/*
	* The MW engine utility for playing audio files (sound effects and music)
	*/
	class AudioManager {
	public:
		/*
		* Disable the copy constructor
		*/
		AudioManager(AudioManager& am) = delete;
		/*
		* Initialize the audio manager, set up a sound source
		*/
		bool init();
		/*
		* Get the gain of all audio
		*/
		float getGain() const { return m_gain; }
		/*
		* Set the gain of all audio
		*/
		void setGain(float gain);
		/*
		* Test whether the music track is currently playing
		*/
		bool isMusicPlaying() {
			return getSourceState(m_musicSourceID) == AL_PLAYING;
		}
		/*
		* Test whether the music track is currently paused
		*/
		bool isMusicPaused() {
			return getSourceState(m_musicSourceID) == AL_PAUSED;
		}
		/*
		* Test whether the music track is currently stopped
		*/
		bool isMusicStopped() {
			return getSourceState(m_musicSourceID) == AL_STOPPED;
		}
		/*
		* Play a music track
		*
		* @param music: A pointer to the music to play
		*/
		void playMusic(const Sound* sound);
		/*
		* Play the most recently played music track
		*/
		void playMusic();
		/*
		* Pause the currently playing music
		*/
		void pauseMusic();
		/*
		* Stop the music track
		*/
		void stopMusic();
		/*
		* Play a sound effect
		*
		* @param sound: A pointer to the sound effect to play
		*/
		void playSound(const Sound* sound);
		/*
		* Stop the music track and all sound effects
		*/
		void stop();
		/*
		* Free this audio manager's memory
		*/
		void destroy();
		/*
		* Get the singleton instance of this class
		*/
		static AudioManager& getInstance() {
			return m_instance;
		}

	private:
		// The OpenAL sound device
		ALCdevice* m_device = nullptr;
		// The OpenAL context
		ALCcontext* m_context = nullptr;
		// The gain of all audio
		float m_gain = 1.0f;
		// The audio source for the current music track
		ALuint m_musicSourceID = 0;
		// The audio sources for sound effects
		std::vector<ALuint> m_effectSources;
		// The singleton instance of this class
		static AudioManager m_instance;

		// Disable the constructor
		AudioManager() {}
		/*
		* Create a new audio source
		*
		* @param looping: Whether the source's audio should loop to the
		* beginning when finished (false by default)
		* @return The ID number of the new audio source
		*/
		ALuint createSource(bool looping = false);
		/*
		* Get the current state of an OpenAL audio source
		*/
		ALint getSourceState(ALuint source);
	};
}

#endif