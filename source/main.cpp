#include <iostream>
#include "board.hpp"
#include "perft.hpp"

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"



int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  
  Board b;
  Board::initStaticMembers();
 
  runPerft(b);


  return 0;

}
