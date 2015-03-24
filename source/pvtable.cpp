#include "pvtable.hpp"
#include "move.hpp"
#include "board.hpp"
#include "log.hpp"

#include <cassert>

// 1MB PVTable
const int PVTable::PvSize = 0x100000 * 1;

// Allocate PVTable on heap
PVTable::PVTable()
{
    m_numEntries = PvSize / sizeof(PVEntry);
    m_numEntries -= 2;
    if (m_pTable != NULL)
        delete[] m_pTable;

    m_pTable = new PVEntry[m_numEntries];

    ClearPvTable();
}

// Free heap memory like a good boy
PVTable::~PVTable() { delete[] m_pTable; }

// Clears pvtable data
void PVTable::ClearPvTable()
{

    for (PVEntry* pvEntry = m_pTable; pvEntry < m_pTable + m_numEntries; pvEntry++) {
        pvEntry->m_posKey = 0;
        pvEntry->m_move = 0;
    }
}

// adds move to pv table
void PVTable::StorePvMove(const Board& b, const int move)
{

    PVEntry entry;
    entry.m_move = move;
    entry.m_posKey = b.m_posHash;

    int index = b.m_posHash % m_numEntries;
    assert(index >= 0 && index <= m_numEntries - 1);

    m_pTable[index] = entry;

#ifndef NDEBUG

    Log* log = Log::getInstance();
    char str[50];
    sprintf(str, "Store pv move %s at %d\n", MoveGen::moveString(move).c_str(), m_numEntries);
    log->writeLine(str);
#endif
}

// Retrieves PV move from table is PV entry is found with same position hash
int PVTable::ProbePvTable(const Board& b)
{

    int index = b.m_posHash % m_numEntries;
    assert(index >= 0 && index <= m_numEntries - 1);

    if (m_pTable[index].m_posKey == b.m_posHash) {
        return m_pTable[index].m_move;
    }

    return 0;
}

// For a given depth, returns the count of number of moves we've inserted
// into pv_array. i.e populates PV array
int PVTable::GetPvLine(const int depth, Board& b)
{

    assert(depth < MAXDEPTH);

    int move = ProbePvTable(b);
    int count = 0;

    while (move != 0 && count < depth) {

        assert(count < MAXDEPTH);

        if (MoveGen::moveExists(b, move)) {
            MoveGen::makeMove(b, move);
            m_pvArray[count++] = move;
        }
        else {
            break;
        }
        move = ProbePvTable(b);
    }

    // Takes back all moves made to find PV array
    while (b.m_ply > 0) {
        MoveGen::takeMove(b);
    }

    return count;
}
