/*
* File: LogManager.cpp
* Author: Keegan MacDonald
* Date: 2021.01.08.2115
*/

#include <time.h>
#include <direct.h>

#include "LogManager.h"

namespace MW {
	void LogManager::init(bool printToConsole) {
		m_printToConsole = printToConsole;
		_mkdir("mwlog/");
		m_logFile.open("mwlog/" + getDate() + ".mwlog");
	}

	void LogManager::destroy() {
		m_logFile.close();
	}

	std::string LogManager::getDate() {
		// Get the current time
		time_t now = time(0);
		tm tstruct;
		localtime_s(&tstruct, &now);
		// Write the time into a buffer in the correct format
		char buffer[32];
		strftime(buffer, sizeof(buffer), "%Y.%m.%d.%H%M.%S", &tstruct);
		return std::string(buffer);
	}
}