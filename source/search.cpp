#include "search.hpp"
#include "move.hpp"
#include "pvtable.hpp"
#include <cassert>
#include <algorithm>
#include <vector>
#include <limits>       // std::numeric_limits
#include <sys/time.h>


#define MATE (std::numeric_limits<int>::min() + 1)



static int GetTimeMs() { 

  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;

}


bool Search::isRepetition(const Board& b) 
{

	for(int index = b.m_hisply - b.m_fiftyMove; index < b.m_hisply-1; ++index) {	
		assert(index >= 0 && index < MAX_GAME_MOVES);		
		if(b.m_posHash == b.m_history[index].posHash) {
			return true;
		}
	}	

	return false;
}

void Search::CheckUp() {
	// .. check if time up, or interrupt from GUI
}

void Search::ClearForSearch(Board& b, SearchInfo& info) {
		
	for(int index = 0; index < 13; ++index) {
		for(int index2 = 0; index2 < TOTAL_SQUARES; ++index2) {
			b.m_searchHistory[index][index2] = 0;
		}
	}
	
	for(int index = 0; index < 2; ++index) {
		for(int index2 = 0; index2 < MAXDEPTH; ++index2) {
			b.m_searchKillers[index][index2] = 0;
		}
	}	
	
	b.m_pvTable.ClearPvTable();	
	b.m_ply = 0;
	
	info.starttime = GetTimeMs();
	info.stopped = 0;
	info.nodes = 0;
	info.fh = 0;
	info.fhf = 0;
}

int Search::Quiescence(int alpha, int beta, Board& b, SearchInfo info) {
	return 0;
}

int Search::AlphaBeta(int alpha, int beta, int depth, Board& b, SearchInfo& info, bool DoNull) {

	assert(b.checkBoard()); 
	
	if(depth == 0) {
		info.nodes++;
		return EvalPosition(b);
	}
	
	info.nodes++;
	
	if(isRepetition(b) || b.m_fiftyMove >= 100) {
		return 0;
	}
	
	if(b.m_ply > MAXDEPTH - 1) {
		return EvalPosition(b);
	}
	
	MoveList list;
	list.genAllMoves(b);
      
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = 0;
	
	std::vector<Move>::iterator itr;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
			
		PickNextMove(itr, list);	
		
        if ( !MakeMove(b,itr->m_move))  {
            continue;
        }
        
		Legal++;
		int Score = -AlphaBeta( -beta, -alpha, depth-1, b, info, true);		
        TakeMove(b);
		
		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info.fhf++;
				}
				info.fh++;	
				return beta;
			}
			alpha = Score;
			BestMove = itr->m_move;
		}		
    }
	
	if(Legal == 0) {
		Colour changeSide = static_cast<Colour>(b.m_side^1);
		if(b.isSquareAttacked(b.m_kingSq[b.m_side],changeSide)) {
			return MATE + b.m_ply;
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		b.m_pvTable.StorePvMove(b, BestMove);
	}
	
	return alpha;
} 

void Search::SearchPosition(Board& b, SearchInfo& info) {
	
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	ClearForSearch(b,info);
	
	// iterative deepening
	for(int currentDepth = 1; currentDepth <= info.depth; ++currentDepth ) {
							// alpha	 beta
		int bestScore = AlphaBeta(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), currentDepth, b, info, true);
		
		// out of time?
		
		pvMoves = b.m_pvTable.GetPvLine(currentDepth, b);
		int bestMove = b.m_pvTable.m_pvArray[0];
		
			
	}
	
	
}

int Search::EvalPosition(const Board& b) {

	Piece pce;

	int score = b.m_material[WHITE] - b.m_material[BLACK];

	bitboard wpBitboard = b.m_pList[wP];
	int numPce = Bitboard::countBits(wpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&wpBitboard); 
		assert(!SQOFFBOARD(sq));
		score += PawnTable[SQ64(sq)];
	}	

	bitboard bpBitboard = b.m_pList[bP];
	numPce = Bitboard::countBits(bpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&bpBitboard); 
		assert(!SQOFFBOARD(sq));
		score -= PawnTable[Mirror64[SQ64(sq)]];
	}	
	
	bitboard wnBitboard = b.m_pList[wN];
	numPce = Bitboard::countBits(wnBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&wnBitboard); 
		assert(!SQOFFBOARD(sq));
		score += KnightTable[SQ64(sq)];
	}	

	bitboard bnBitboard = b.m_pList[bN];
	numPce = Bitboard::countBits(bnBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&bnBitboard); 
		assert(!SQOFFBOARD(sq));
		score -= KnightTable[Mirror64[SQ64(sq)]];
	}			
	
	bitboard wbBitboard = b.m_pList[wB];
	numPce = Bitboard::countBits(wbBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&wbBitboard); 
		assert(!SQOFFBOARD(sq));
		score += BishopTable[Mirror64[SQ64(sq)]];
	}	

	bitboard bbBitboard = b.m_pList[bB];
	numPce = Bitboard::countBits(bbBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&bbBitboard); 
		assert(!SQOFFBOARD(sq));
		score -= BishopTable[Mirror64[SQ64(sq)]];
	}	

	bitboard wrBitboard = b.m_pList[wR];
	numPce = Bitboard::countBits(wrBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&wrBitboard); 
		assert(!SQOFFBOARD(sq));
		score += RookTable[SQ64(sq)];
	}	

	bitboard brBitboard = b.m_pList[bR];
	numPce = Bitboard::countBits(brBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq = Bitboard::popBit(&brBitboard); 
		assert(!SQOFFBOARD(sq));
		score -= RookTable[Mirror64[SQ64(sq)]];
	}	
	
	if(b.m_side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}

void Search::PickNextMove(std::vector<Move>::iterator move, MoveList& list) {


    std::vector<Move>::iterator result = std::max_element(move, list.m_move_vec.end(), 
    	                                 [](Move a, Move b){
                                             return a.m_score > b.m_score;
    	                                  });

	std::iter_swap(result, move);
}