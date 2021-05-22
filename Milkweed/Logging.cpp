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

	unsigned int StringUtils::Contains(const std::string& str,
		const std::string& reg) {
		if (reg.empty() || str.length() < reg.length()) {
			return 0;
		}
		else if (str == reg) {
			return 1;
		}

		unsigned int matchCount = 0;
		for (unsigned int i = 0; i < str.length() - reg.length(); i++) {
			bool match = true;
			for (unsigned int j = 0; j < reg.length(); j++) {
				if (str[i + j] != reg[j]) {
					match = false;
					break;
				}
			}
			if (match) {
				matchCount++;
			}
		}

		return matchCount;
	}

	bool StringUtils::Part(const std::string& str, std::string& part,
		unsigned int a, unsigned int b) {
		part.clear();
		if ((int)b - (int)a < 0) {
			return false;
		}
		if (a >= str.size() || b >= str.size()) {
			return false;
		}
		part.resize(b - a + 1);
		for (unsigned int i = 0; i <= (b - a); i++) {
			part[i] = str[i + a];
		}
		return true;
	}

	bool StringUtils::Split(const std::string& str, const std::string& reg,
		std::vector<std::string>& parts) {
		parts.clear();

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < str.length() - reg.length() + 1; i++) {
			bool test = true;
			for (unsigned int j = 0; j < reg.length(); j++) {
				if (str[i + j] != reg[j]) {
					test = false;
					break;
				}
			}
			if (test) {
				indices.push_back(i);
			}
		}
		if (indices.empty()) {
			return false;
		}

		int lastIndex = -1;
		for (unsigned int index : indices) {
			std::string s;
			if (lastIndex == -1) {
				Part(str, s, 0, index - 1);
			}
			else {
				Part(str, s, lastIndex + reg.size(), index - 1);
			}
			lastIndex = (int)index;
			if (!s.empty()) {
				parts.push_back(s);
			}
		}
		std::string s;
		Part(str, s, lastIndex + reg.size(), str.size() - 1);
		if (!s.empty()) {
			parts.push_back(s);
		}

		return true;
	}
}