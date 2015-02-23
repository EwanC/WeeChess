#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "move.hpp"
#include "perft.hpp"
#include "board.hpp"

bool runPerft(Board& b, const int depth)
{
   std::ifstream infile("../perftsuite.epd");
   std::string line;

   int count = 0;
   bool pass = true;

   assert(depth > 0 && depth < 7);
   while (std::getline(infile, line))
   {

       b.parseFen(line.c_str());

       std::size_t pos =0;      // expected leaf nodes
       
       for(int i = 0; i<depth;++i){
          pos = line.find(";",pos+1);      // expected leaf nodes
       }

      int expected = atoi((line.substr(pos+3)).c_str());

      uint32_t result = PerftTest(depth,b);
      std::cout << "\nTest "<<std::dec<<count++<<" to depth "<<depth;
      std::cout << " ===> expected: "<<expected<< ", calculated "<<result; 

      if(expected == result){
        std::cout << " PASS\n";
      }
      else{
        std::cout << " FAIL\n";
        pass = false;
      }

   }

   if(pass)
    std::cout << "\nPerft Success\n";
   else
    std::cout << "\nPerft Failure\n";

   return pass;
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


uint32_t PerftTest(const int depth, Board& b,bool verbose) {

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
	
    return leafNodes;
}










