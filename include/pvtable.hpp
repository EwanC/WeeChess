#ifndef PVTABLE_H
#define PVTABLE_H

#include <stdint.h> //Standard Integer Types
#include <vector>
#include "types.hpp"

class Board;

struct PVEntry{
	uint64_t m_posKey;
	int m_move;
};


class PVTable{ 

   public:

   std::vector<PVEntry> m_pTable;
   int m_pvArray[MAXDEPTH];

   void ClearPvTable();
   void StorePvMove(const Board& b, const int move);
   int ProbePvTable(const Board& b);
   int GetPvLine(const int depth, Board& b);

};



#endif