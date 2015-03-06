#ifndef PVTABLE_H
#define PVTABLE_H

#include <stdint.h> //Standard Integer Types
#include <vector>
#include "types.hpp"

class Board;

// Principle variation entry    
// when a move is found in the search that beats alpha,
// store best move and position key
struct PVEntry{
	uint64_t m_posKey;  // hash key 
	int m_move;         // move
};


class PVTable{ 

   public:

   std::vector<PVEntry> m_pTable;   // vector of PV entries representing pvtable
                                    // TODO make more effiecent. hash table?

   int m_pvArray[MAXDEPTH];         // PV move at each level of the tree 

   void ClearPvTable();
   void StorePvMove(const Board& b, const int move); // Stores move in table
   int ProbePvTable(const Board& b);                 
   int GetPvLine(const int depth, Board& b);

};



#endif