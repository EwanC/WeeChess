#include "pvtable.hpp"
#include "move.hpp"
#include "board.hpp"

#include <algorithm>
#include <cassert>


void PVTable::ClearPvTable() {

  m_pTable.clear();

}

void PVTable::StorePvMove(const Board& b, const int move) {

	PVEntry entry;
	entry.m_move = move;
	entry.m_posKey = b.m_posHash;

	m_pTable.push_back(entry);
}

int PVTable::ProbePvTable(const Board& b) {

    std::vector<PVEntry>::iterator itr;

    itr = std::find_if (m_pTable.begin(), m_pTable.end(),
                        [&b](PVEntry& e){
                            return b.m_posHash == e.m_posKey;
    	                });

    if(itr == m_pTable.end())  
        return 0;
    else
    	return itr->m_move;
	
}

int PVTable::GetPvLine(const int depth, Board& b) {

	assert(depth < MAXDEPTH);

	int move = ProbePvTable(b);
	int count = 0;
	
	while(move != 0 && count < depth) {
	
		assert(count < MAXDEPTH);
	
		if( MoveExists(b, move) ) {
			MakeMove(b, move);
			m_pvArray[count++] = move;
		} else {
			break;
		}		
		move = ProbePvTable(b);	
	}
	
	while(b.m_ply > 0) {
		TakeMove(b);
	}
	
	return count;
	
}

