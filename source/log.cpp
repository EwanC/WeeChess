#include "log.hpp"

Log *Log::m_instance = 0;


Log::Log(){

	  m_logfile.open ("log.txt");

}

Log::~Log(){

	  m_logfile.close();
	  
}

// Singleton class
Log* Log::getInstance()
{
    if (!m_instance)
        m_instance = new Log;
    return m_instance;
}

void Log::writeLine(std::string str)
{
	  m_logfile << str;
}

void Log::writeLine(char* str)
{
	 m_logfile << str;

}
