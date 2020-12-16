/*
* File: AudioManager.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.12.16.1146
*/

#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace MW {
	/*
	* The MW engine utility for playing audio files (sound effects and music)
	*/
	class AudioManager {
	public:
		/*
		* Initialize the audio manager, set up a sound source
		*/
		bool init();
		/*
		* Free the audio manager's memory
		*/
		void destroy();

	private:
		ALCdevice* m_device = nullptr;
		ALuint m_sourceID = 0;
	};
}