/*
* File: LogManager.cpp
* Author: Keegan MacDonald
* Created: 2021.01.08
*/

#include <time.h>
#include <direct.h>

#include "Logging.h"

namespace Milkweed {
	LogManager LogManager::m_instance;

	void LogManager::init(const std::string& dirName, bool printToConsole) {
		// Set whether to print messages to the console
		m_printToConsole = printToConsole;
		
		// Create the logs directory if not present and generate the log file
		int result = _mkdir(dirName.c_str());
		m_logFile.open(dirName + "/" + getDate() + ".mwlog");
	}

	std::string LogManager::getDate() const {
		// Get the current time
		time_t now = time(0);
		tm tstruct;
		localtime_s(&tstruct, &now);
		// Write the time into a buffer in the appropriate format
		char buffer[32];
		strftime(buffer, sizeof(buffer), m_dateFormat.c_str(), &tstruct);
		return std::string(buffer);
	}

	void LogManager::destroy() {
		// Close the log file
		m_logFile.close();
	}
}