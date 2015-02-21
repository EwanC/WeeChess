#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "move.hpp"
#include "perft.hpp"
#include "board.hpp"

void runPerft(Board& b)
{
   std::ifstream infile("../perftsuite.epd");
   std::string line;
   while (std::getline(infile, line))
   {
       b.parseFen(line.c_str());
       PerftTest(1,b);
   }

}






uint32_t Perft(const int depth, Board& b) {

    assert(b.checkBoard());  

	if(depth == 0) {
        return 1;
    }	

    MoveList list;
    list.genAllMoves(b);
      
    std::vector<Move>::iterator itr;

    int nodesAccum = 0;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.begin(); itr++) {	
       
        if (!MakeMove(b,itr->m_move))  {
            continue;
        }
        nodesAccum += Perft(depth - 1, b);
        TakeMove(b);
    }

    return nodesAccum;
}


void PerftTest(const int depth, Board& b) {

    assert(b.checkBoard());

	b.printBoard();
	std::cout << "\nStarting Test To Depth:"<<depth<<std::endl;	
	uint32_t leafNodes = 0;
	
    MoveList list;
    list.genAllMoves(b);	
    
    std::vector<Move>::iterator itr;
    int moveNum = 0;
    for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.begin(); itr++) { 
        uint32_t move = itr->m_move;
        if ( !MakeMove(b,move))  {
            continue;
        }
        uint32_t calcNodes = Perft(depth - 1, b);
        leafNodes += calcNodes;
        TakeMove(b);        

        std::cout << "move "<< moveNum++ << " : "<<itr->moveString() << " : "<< calcNodes<< std::endl;
    }
	
	std::cout<<"\nTest Complete : "<<leafNodes<<" leaf nodes visited\n";

    return;
}











