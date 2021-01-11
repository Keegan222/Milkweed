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
		void init();
		template <typename T>
		friend LogManager& operator << (LogManager& ls, T& t) {
			// TODO: Print timestamps at the beginning of each message
			if (!std::is_standard_layout<T>::value) {
				// The type T can't be written to the console
				return ls;
			}
#ifdef _DEBUG
			std::cout << t;
#endif
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
		std::ofstream m_logFile;

		std::string getDate();
	};
}