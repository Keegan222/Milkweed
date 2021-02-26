/*
* File: AudioManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.12.16
*/

#ifndef MW_AUDIO_H
#define MW_AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>

#include "Resources.h"

namespace Milkweed {
	/*
	* The MW framework utility for playing audio files (sound effects and music)
	*/
	class AudioManager {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		AudioManager(AudioManager& am) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static AudioManager& getInstance() {
			return m_instance;
		}

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

	private:
		// The singleton instance of this class
		static AudioManager m_instance;
		/*
		* The constructor is disabled for this class
		*/
		AudioManager() {}

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

		/*
		* Create a new OpenAL audio source to play a sound effect
		*/
		ALuint createSource(bool looping = false);
		/*
		* Get the current state of an OpenAL audio source
		*/
		ALint getSourceState(ALuint source);
	};
}

#endif