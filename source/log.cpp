#include "log.hpp"

Log* Log::m_instance = 0; // Singleton instance

// Open log file debug info is written to
Log::Log()
{
    m_logfile.open("log.txt");
}

// close log file
Log::~Log()
{
    m_logfile.close();
}

// Singleton class
Log* Log::getInstance()
{
    if (!m_instance)
        m_instance = new Log;
    return m_instance;
}

// Write line to log file
void Log::writeLine(std::string str)
{
    m_logfile << str;
}

// Write line to log file
void Log::writeLine(char* str)
{
    m_logfile << str;
}
