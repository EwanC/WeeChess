#include <iostream>
#include "protocols.hpp"
#include <cstring>


void printWelcome()
{
    std::cout <<std::endl;
    std::cout << "| |  | |    | |                          | |        | |  | |         /  __ \\ |                  \n";
    std::cout <<"| |  | | ___| | ___ ___  _ __ ___   ___  | |_ ___   | |  | | ___  ___| /  \\/ |__   ___  ___ ___ \n";
    std::cout <<"| |/\\| |/ _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  | |/\\| |/ _ \\/ _ \\ |   | '_ \\ / _ \\/ __/ __|\n";
    std::cout <<"\\  /\\  /  __/ | (_| (_) | | | | | |  __/ | || (_) | \\  /\\  /  __/  __/ \\__/\\ | | |  __/\\__ \\__ \\\n";
    std::cout <<" \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/   \\/  \\/ \\___|\\___|\\____/_| |_|\\___||___/___/\n";

    std::cout << "\nSelect protocol from 'uci','xboard', and 'cli': ";

}

int main(int argc, char* argv[])
{

  Board b;
  Board::initStaticMembers();
    
  SearchInfo info;
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  
  printWelcome();


  char line[256];
  while (true) {
    memset(&line[0], 0, sizeof(line));

    fflush(stdout);
    if (!fgets(line, 256, stdin))
      continue;
    if (line[0] == '\n')
      continue;

    // Pick protocl to interact with engine, TODO parse these from invoke args
    if (!strncmp(line, "uci",3)) {      
      UCI::UCILoop(b, info);
      if(info.quit == true) break;
      continue;
    } else if (!strncmp(line, "xboard",6))  {
      XBoard::XBoardLoop(b, info);
      if(info.quit == true) break;
      continue;
    } else if (!strncmp(line, "cli",3))  {
      consoleLoop(b, info);
      if(info.quit == true) break;
      continue;
    } else if(!strncmp(line, "quit",4)) {
      break;
    }
  }
  
  return 0;

}
