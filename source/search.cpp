#include "search.hpp"
#include <cassert>

bool isRepetition(const Board& b) 
{

	for(int index = b.m_hisply - b.m_fiftyMove; index < b.m_hisply-1; ++index) {	
		assert(index >= 0 && index < MAX_GAME_MOVES);		
		if(b.m_posHash == b.m_history[index].posHash) {
			return true;
		}
	}	

	return false;
}

