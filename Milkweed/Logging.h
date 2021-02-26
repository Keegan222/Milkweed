/*
* File: LogManager.h
* Author: Keegan MacDonald
* Created: 2021.01.08
*/

#ifndef MW_LOGGING_H
#define MW_LOGGING_H

#include <fstream>
#include <iostream>

namespace Milkweed {
	/*
	* A system for managing log files and writing messages to a file and the
	* console, wrapper for an std::ofstream
	*/
	class LogManager {
	public:
		/*
		* The copy constructor is disabled for this class
		*/
		LogManager(LogManager& lm) = delete;
		/*
		* Get the singleton instance of this class
		*/
		static LogManager& getInstance() {
			return m_instance;
		}

		/*
		* Set up the file for this log manager to write messages into
		* 
		* @param dirName: The desired directory to place the log file in
		* @param printToConsole: Whether to print log messages to the console
		* (true by default)
		*/
		void init(const std::string& dirName, bool printToConsole = true);
		/*
		* Override for the bitshift left operator to push data to the console
		* and log file simultaneously
		*/
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
		/*
		* Get the current date and time in this log manager's current date
		* format
		*/
		std::string getDate() const;
		/*
		* Get the date format this log manager is currently using to generate
		* new log files and execute its getDate() function
		*/
		const std::string& getDateFormat() const { return m_dateFormat; }
		/*
		* Set the date format for this log manager to use when generating log
		* files and executing its getDate() function
		*/
		void setDateFormat(const std::string& dateFormat) {
			m_dateFormat = dateFormat;
		}
		/*
		* Close this log manager's log file and free its memory
		*/
		void destroy();

	private:
		// The singleton instance of this class
		static LogManager m_instance;
		/*
		* The constructor is disabled for this class
		*/
		LogManager() {}

		// Whether to print new messages to the console with std::cout
		bool m_printToConsole = true;
		// The file to print messages logged with the << operator to
		std::ofstream m_logFile;
		// The format to print the date in
		std::string m_dateFormat = "%Y.%m.%d.%H%M.%S";
	};
}

#endif