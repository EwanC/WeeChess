#include "search.hpp"
#include <cassert>
#include <algorithm>
#include <vector>


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

	assert(b.CheckBoard()); 
	
	if(depth == 0) {
		info.nodes++;
		return EvalPosition(b);
	}
	
	info.nodes++;
	
	if(IsRepetition(b) || b.fiftyMove >= 100) {
		return 0;
	}
	
	if(b.ply > MAXDEPTH - 1) {
		return EvalPosition(b);
	}
	
	MoveList list;
	list.genAllMoves(b);
      
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE;
	
	for(int MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
			
		PickNextMove(MoveNum, list);	
		
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -AlphaBeta( -beta, -alpha, depth-1, pos, info, TRUE);		
        TakeMove(pos);
		
		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;	
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}		
    }
	
	if(Legal == 0) {
		if(SqAttacked(pos->KingSq[pos->side],pos->side^1,pos)) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}
	
	return alpha;
} 

void Search::SearchPosition(Board& b, SearchInfo& info) {
	
	int bestMove = NOMOVE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	ClearForSearch(b,info);
	
	// iterative deepening
	for(int currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
							// alpha	 beta
		int bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, b, info, true);
		
		// out of time?
		
		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];
		
		printf("Depth:%d score:%d move:%s nodes:%ld ",
			currentDepth,bestScore,PrMove(bestMove),info->nodes);
			
		pvMoves = GetPvLine(currentDepth, pos);	
		printf("pv");		
		for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
			printf(" %s",PrMove(pos->PvArray[pvNum]));
		}
		printf("\n");
		printf("Ordering:%.2f\n",(info->fhf/info->fh));
	}
	
	
}

int Search::EvalPosition(const Board& b) {

	Piece pce;
	int sq;

	int score = b.m_material[WHITE] - b.m_material[BLACK];
	
	pce = wP;	
	for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score += PawnTable[SQ64(sq)];
	}	

	pce = bP;	
	for(int pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score -= PawnTable[Mirror64[SQ64(sq)]];
	}	
	
	pce = wN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score += KnightTable[SQ64(sq)];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score -= KnightTable[Mirror64[SQ64(sq)];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score += BishopTable[Mirror64[SQ64(sq)]];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score -= BishopTable[Mirror64[SQ64(sq)]];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		assert(!SQOFFBOARD(sq));
		score += RookTable[SQ64(sq)];
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
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


    std::vector<Move>::iterator result = std::max_element(move, list.end(), 
    	                                 [](Move a, Move b){
                                             return a->m_score > b.m_score;
    	                                  })

	std::iter_swap(result, move);
}