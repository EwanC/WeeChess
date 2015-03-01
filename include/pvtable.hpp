#ifndef PVTABLE_H
#define PVTABLE_H

#include <stdint.h> //Standard Integer Types
#include <vector>

#include "board.hpp"

#define MAXDEPTH 64

struct PVEntry{
	uint64_t m_posKey;
	int m_move;
};


class PVTable{ 

   std::vector<PVEntry> m_pTable;
   int m_pvArray[MAXDEPTH];

   public:
   void ClearPvTable();
   void StorePvMove(const Board& b, const int move);
   int ProbePvTable(const Board& b);
   int GetPvLine(const int depth, Board& b);

};



#endif