#ifndef LOG_H
#define LOG_H


#include <fstream>
#include <string>

class Log{

public:
	static Log* getInstance();
  
    void writeLine(std::string str);
    void writeLine(char* str);

private:

    std::ofstream m_logfile;

    static Log* m_instance;

	Log();                   // Constructor? (the {} brackets) are needed here.    
    Log(Log const&);              // Don't Implement
    void operator=(Log const&); // Don't implement
    ~Log();


};










#endif // LOG_H