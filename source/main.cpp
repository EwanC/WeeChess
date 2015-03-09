#include <iostream>
#include "board.hpp"
#include "perft.hpp"
#include "move.hpp"
#include "search.hpp"

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -" //MATE in 3 position
//#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  
  Board b;
  Board::initStaticMembers();

  // runPerft(b,4);
  b.parseFen(WAC1);

  char input[6];

  SearchInfo info;
  while(true)
  {
     b.printBoard();
     std::cout << "\nPlease enter a move: ";
     std::cin >> input;

     if(input[0] == 'q'){
     	break;
     } else if(input[0] == 't'){
     	TakeMove(b);
     }
     else if(input[0]=='s') {
      info.depth = 4;

      Search::SearchPosition(b, info);
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
