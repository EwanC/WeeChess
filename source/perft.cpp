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

   int count = 0;
   int depth = 1;
   while (std::getline(infile, line))
   {

       b.parseFen(line.c_str());
       std::cout << "\nTest "<<count++<<" to depth:"<<depth<<std::endl; 

       PerftTest(depth,b);
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
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) {	
       
        if (!MakeMove(b,itr->m_move))  {
            continue;
        }
        nodesAccum += Perft(depth - 1, b);
        TakeMove(b);
    }

    return nodesAccum;
}


void PerftTest(const int depth, Board& b,bool verbose) {

    assert(b.checkBoard());

    if(verbose)
	    b.printBoard();
	
    uint32_t leafNodes = 0;
	
    MoveList list;
    list.genAllMoves(b);	
    
    std::vector<Move>::iterator itr;
    int moveNum = 0;
    for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) { 
        uint32_t move = itr->m_move;
        if ( !MakeMove(b,move))  {
            continue;
        }
        uint32_t calcNodes = Perft(depth - 1, b);
        leafNodes += calcNodes;
        TakeMove(b);        

        if(verbose)
            std::cout << "move "<< moveNum++ << " : "<<itr->moveString() << " : "<< calcNodes<< std::endl;
    }
	
	std::cout<<"Test Complete : "<<leafNodes<<" leaf nodes visited\n";

    return;
}











