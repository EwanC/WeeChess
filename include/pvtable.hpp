#ifndef PVTABLE_H
#define PVTABLE_H

#include <stdint.h> //Standard Integer Types
#include "types.hpp"

class Board;

// Principle variation entry
// when a move is found in the search that beats alpha,
// store best move and position key
struct PVEntry {
    uint64_t m_posKey; // hash key
    int m_move;        // move
};

class PVTable {

    static const int PvSize; // Heap size of PVTable

  public:
    PVTable();  // Allocate on heap
    ~PVTable(); // Free from heap

    PVEntry* m_pTable; // heap array of PV entries representing pvtable
    int m_numEntries;

    int m_pvArray[MAXDEPTH]; // PV move at each level of the tree

    void ClearPvTable();                              // Clear PVTable entries
    void StorePvMove(const Board& b, const int move); // Stores move in table
    int ProbePvTable(const Board& b); // Retrieves PV move from table is PV entry is found with same
                                      // position hash
    int
    GetPvLine(const int depth,
              Board& b); // For a given depth, returns the count of number of moves we've inserted
    // into pv_array. i.e populates PV array
};

#endif