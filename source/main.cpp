#include <iostream>
#include "board.hpp"
#include "move.hpp"

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"



int main(int argc, char* argv[])
{
  std::cout << "Welcome to WeeChess\n";
  
  Board b;
  Board::initStaticMembers();
  b.parseFen(FEN1);
  b.printBoard();
  std::cout << "success : "<< b.checkBoard()<<std::endl;

  Move m;
  // b.parseFen(FEN2);
  // b.printBoard();
  // std::cout << "success : "<< b.checkBoard()<<std::endl;


  // b.parseFen(FEN3);
  // b.printBoard();
  // std::cout << "success : "<< b.checkBoard()<<std::endl;

  // for(int i = 0; i < 13; i++)
  // {
  //   std::cout << Board::PceChar[i] << " Bitboard:\n";
  //   Bitboard::printBitboard(b.m_pList[i]);
  // }


  return 0;

}
