/*
* File: LogManager.h
* Author: Keegan MacDonald
* Created: 2021.01.08
*/

#ifndef MW_LOGGING_H
#define MW_LOGGING_H

#include <fstream>
#include <iostream>
#include <vector>

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
		friend LogManager& operator , (LogManager& ls, const T& t) {
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

	/*
	* Class for utility functions used to parse strings
	*/
	class StringUtils {
	public:
		/*
		* Test whether a string contains a regular expression.
		* 
		* @param str: The string to test.
		* @param reg: The regular expression to look for in str.
		* @return The number of times the regular expression appears in str.
		*/
		static unsigned int Contains(const std::string& str,
			const std::string& reg);
		/*
		* Populate a string with part of another.
		* 
		* @param str: The string to take a substring of.
		* @param part: The string to populate.
		* @param a: The starting index of the substring.
		* @param b: The ending index of the substring.
		* @return Whether a substring was found.
		*/
		static bool Part(const std::string& str, std::string& part,
			unsigned int a, unsigned int b);
		/*
		* Split a string around a regular expression.
		* 
		* @param str: The string to split.
		* @param reg: The regular expression to split str on.
		* @param parts: A reference to the vector to populate with the portions
		* of the string resulting from the split.
		* @return Whether the regular expression appeared at least once in str.
		*/
		static bool Split(const std::string& str, const std::string& reg,
			std::vector<std::string>& parts);
	};
}

#endif