/*!
 \file Log.cpp
 \brief Class to manage Log
 \author Màrius Montón
 \date Aug 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Log.h"

Log* Log::getInstance() {
	if (instance == 0) {
		instance = new Log("Log.txt");
	}

	return instance;
}

Log::Log(const char *filename) {
	m_stream.open(filename);
	currentLogLevel = Log::INFO;
}

void Log::SC_log(std::string msg, enum LogLevel level) {

	if (level <= currentLogLevel) {
		m_stream << "time " << sc_core::sc_time_stamp() << ": " << msg
				<< "\n";
	}
}

std::ofstream& Log::SC_log(enum LogLevel level) {

	if (level <= currentLogLevel) {
		m_stream << "time " << sc_core::sc_time_stamp() << ": ";
		return m_stream;
	} else {
		return m_sink;
	}

}

void Log::setLogLevel(enum LogLevel newLevel) {
	std::cout << "LogLevel set to " << newLevel << "\n";
	currentLogLevel = newLevel;
}

enum Log::LogLevel Log::getLogLevel() {
	return currentLogLevel;
}

Log *Log::instance = 0;
