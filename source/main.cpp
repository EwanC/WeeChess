#include <iostream>
#include "board.hpp"
#include "perft.hpp"
#include "move.hpp"

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1" //MATE in 3 position

int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  
  Board b;
  Board::initStaticMembers();
  
  // runPerft(b,4);
  b.parseFen(START_FEN);

  char input[6];
  while(true)
  {
     b.printBoard();
     std::cout << "\nPlease enter a move: ";
     std::cin >> input;

     if(input[0] == 'q'){
     	break;
     } else if(input[0] == 't'){
     	TakeMove(b);
     	continue;
     } else{
        uint32_t move  = parseMove(input, b);
        if(move == 0){
           std::cout <<"\nIncorrect Move";
        }else{
        	MakeMove(b,move);
        }

     }

  }

  return 0;

}
