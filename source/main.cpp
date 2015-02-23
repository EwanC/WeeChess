#include <iostream>
#include "board.hpp"
#include "perft.hpp"

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"



int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  
  Board b;
  Board::initStaticMembers();
 
  #if 0 
  runPerft(b,4);
  #endif

  return 0;

}
