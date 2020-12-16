/*
* File: AudioManager.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Date: 2020.12.16.1156
*/

#include "MW.h"

namespace MW {
	bool AudioManager::init() {
		m_device = alcOpenDevice(nullptr);
		if (m_device == nullptr) {
			return false;
		}
		App::Log("Opened ALC device");

		return true;
	}

	void AudioManager::destroy() {
		alcCloseDevice(m_device);
		m_device = nullptr;
		App::Log("Closed ALC device");
	}
}