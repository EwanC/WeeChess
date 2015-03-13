#include <iostream>
#include "uci.hpp"

int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  UCI::GameLoop();
  
  return 0;

}
