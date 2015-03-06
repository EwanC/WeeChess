#include "search.hpp"
#include "move.hpp"
#include "pvtable.hpp"
#include <cassert>
#include <algorithm>
#include <vector>
#include <limits>       // std::numeric_limits
#include <sys/time.h>


#define MATE (std::numeric_limits<int>::max() - 1) // Board evaluation score for mate

// Returns current time.
static int GetTimeMs() { 

  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;

}

// Check for repition of current board position in last fifty moves
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

// Clears aphaBeta heuristics as well as seachinfo and PVTable
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

// Reduces horizon effect
int Search::Quiescence(int alpha, int beta, Board& b, SearchInfo info) {
	return 0;
}

// Alpa beta search, returns board score
int Search::AlphaBeta(int alpha, int beta, int depth, Board& b, SearchInfo& info, bool DoNull) {

	assert(b.checkBoard()); 
	
	if(depth == 0) {
		info.nodes++;  
		return EvalPosition(b);
	}
	
	info.nodes++; // increment nodes visited
	
	// position is draw.
	if(isRepetition(b) || b.m_fiftyMove >= 100) {
		return 0;
	}
	
	// If we've searched to deep
	if(b.m_ply > MAXDEPTH - 1) {
		return EvalPosition(b);
	}
	
	// Generate all moves for board
	MoveList list;
	list.genAllMoves(b);
      
	int Legal = 0;   // Incremented on legal moves

	int OldAlpha = alpha; // Record what alpha is before loop
	int BestMove = 0;     // Best move found

    // Loop over all moves in move list	
	std::vector<Move>::iterator itr;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
			
		PickNextMove(itr, list);	
		
		// Not legal move
        if ( !MakeMove(b,itr->m_move))  {
            continue;
        }
        
		Legal++;

		// Nega Max, flip bounds for opposition colours
		int Score = -AlphaBeta( -beta, -alpha, depth-1, b, info, true);	

		// Take back move	
        TakeMove(b);
		
		// Improved on alpha
		if(Score > alpha) {
			// Beta cutoff
			if(Score >= beta) {
				if(Legal==1) {
					info.fhf++;
				}
				info.fh++;	
				return beta;  // return beta
			}
			// New best move
			alpha = Score;
			BestMove = itr->m_move; 
		}		
    }
	
	// No legal moves found, checkmate of draw
	if(Legal == 0) {
		Colour changeSide = static_cast<Colour>(b.m_side^1);
		if(b.isSquareAttacked(b.m_kingSq[b.m_side],changeSide)) { 
			return -MATE + b.m_ply;
		} else {
			return 0;
		}
	}
	
	// We improved alpha, store best move for position
	if(alpha != OldAlpha) {
		b.m_pvTable.StorePvMove(b, BestMove);
	}
	
	return alpha;
} 

// Iterative deepening, searching with alpha beta
void Search::SearchPosition(Board& b, SearchInfo& info) {
	
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	// Reset for current search
	ClearForSearch(b,info);
	
	// iterative deepening for each depth
	for(int currentDepth = 1; currentDepth <= info.depth; ++currentDepth ) {

		int bestScore = AlphaBeta(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), currentDepth, b, info, true);
		
		// out of time?

		pvMoves = b.m_pvTable.GetPvLine(currentDepth, b); // populate pv array
		int bestMove = b.m_pvTable.m_pvArray[0];          // find best move in current position, at depth 0
		
		
	}
	
	
}

// Used Piece tabled to evaulate position
int Search::EvalPosition(const Board& b) {

	Piece pce;
 
    // material score
	int score = b.m_material[WHITE] - b.m_material[BLACK];

    // White pawns
	bitboard wpBitboard = b.m_pList[wP];
	int numPce = Bitboard::countBits(wpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wpBitboard); 
		score += PawnTable[sq64];
	}	

    // Black pawns
	bitboard bpBitboard = b.m_pList[bP];
	numPce = Bitboard::countBits(bpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&bpBitboard); 
		score -= PawnTable[Mirror64[sq64]];
	}	
	
	// White Knight
	bitboard wnBitboard = b.m_pList[wN];
	numPce = Bitboard::countBits(wnBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wnBitboard); 
		score += KnightTable[sq64];
	}	

    // Black Knight
	bitboard bnBitboard = b.m_pList[bN];
	numPce = Bitboard::countBits(bnBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&bnBitboard); 
		score -= KnightTable[Mirror64[sq64]];
	}			
	
	// White Bishop
	bitboard wbBitboard = b.m_pList[wB];
	numPce = Bitboard::countBits(wbBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wbBitboard); 
		score += BishopTable[Mirror64[sq64]];
	}	

    // Black Bishop
	bitboard bbBitboard = b.m_pList[bB];
	numPce = Bitboard::countBits(bbBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&bbBitboard); 
		score -= BishopTable[Mirror64[sq64]];
	}	

    // White Rook
	bitboard wrBitboard = b.m_pList[wR];
	numPce = Bitboard::countBits(wrBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wrBitboard); 
		score += RookTable[sq64];
	}	

    // Black Rook
	bitboard brBitboard = b.m_pList[bR];
	numPce = Bitboard::countBits(brBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&brBitboard); 
		score -= RookTable[Mirror64[sq64]];
	}	
	
	if(b.m_side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}

// Finds the best scoring move in the move list and swaps with current move
void Search::PickNextMove(std::vector<Move>::iterator move, MoveList& list) {


    std::vector<Move>::iterator result = std::max_element(move, list.m_move_vec.end(), 
    	                                 [](Move a, Move b){
                                             return a.m_score > b.m_score;
    	                                  });

	std::iter_swap(result, move);
}