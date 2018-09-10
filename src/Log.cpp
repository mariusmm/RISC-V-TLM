#include "Log.h"

Log* Log::getInstance()
{
  if (instance == 0)
  {
      instance = new Log("Log.txt");
  }

  return instance;
}

Log::Log(const char* filename) {
  m_stream.open(filename);
  currentLogLevel = Log::INFO;
}

void Log::SC_log(std::string msg, enum LogLevel level) {
  if (level >= currentLogLevel) {
    m_stream << "time " << sc_core::sc_time_stamp() << ": " << msg << std::endl;
  }
}

std::ofstream& Log::SC_log(enum LogLevel level) {
  if (level >= currentLogLevel) {
    m_stream << "time " << sc_core::sc_time_stamp() << ": ";
  }

  return m_stream;
}

void Log::setLogLevel(enum LogLevel newLevel) {
  currentLogLevel = newLevel;
}

Log* Log::instance = 0;
