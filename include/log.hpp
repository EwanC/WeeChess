#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>

// Singleton class used to log data to a file for debugging
class Log {

  public:
    static Log* getInstance();

    void writeLine(std::string str);
    void writeLine(char* str);

  private:
    std::ofstream m_logfile;

    static Log* m_instance;

    Log();
    Log(Log const&);
    void operator=(Log const&);
    ~Log();
};

#endif // LOG_H