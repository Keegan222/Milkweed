/*
* File: LogManager.h
* Author: Keegan MacDonald
* Date: 2021.01.08.2110
*/

#pragma once

#include <fstream>
#include <iostream>

namespace MW {
	class LogManager {
	public:
		void init(bool printToConsole = true);
		template <typename T>
		friend LogManager& operator << (LogManager& ls, const T& t) {
			// TODO: Print timestamps at the beginning of each message
			if (ls.m_printToConsole) {
				std::cout << t;
			}

			if (ls.m_logFile.fail()) {
				// The logging file is closed
				return ls;
			}
			// The log file hasn't failed, write the log to it
			ls.m_logFile << t;

			return ls;
		}
		void destroy();

	private:
		bool m_printToConsole = true;
		std::ofstream m_logFile;

		std::string getDate();
	};
}