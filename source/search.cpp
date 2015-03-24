#include "search.hpp"
#include "move.hpp"
#include "pvtable.hpp"
#include "log.hpp"
#include "protocols.hpp"
#include <cassert>
#include <algorithm>
#include <vector>
#include <sys/time.h>


#define MATE (29000) // Board evaluation score for mate
#define INF  (30000)

namespace Search
{
	
uint64_t FileBBMask[8];
uint64_t RankBBMask[8];

uint64_t BlackPassedMask[64];
uint64_t WhitePassedMask[64];
uint64_t IsolatedMask[64];

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

}

void Search::InitEvalMasks()
{

	for(int sq = 0; sq < 8; ++sq) {
        Search::FileBBMask[sq] = 0;
		Search::RankBBMask[sq] = 0;
	}

	for(int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            int sq = r * 8 + f;
            Search::FileBBMask[f] |= (1 << sq);
            Search::RankBBMask[r] |= (1 << sq);
        }
	}

	for(int sq = 0; sq < 64; ++sq) {
		Search::IsolatedMask[sq] = 0;
		Search::WhitePassedMask[sq] = 0;
		Search::BlackPassedMask[sq] = 0;
    }

	for(int sq = 0; sq < 64; ++sq) {
		int tsq = sq + 8;

        while(tsq < 64) {
            Search::WhitePassedMask[sq] |= (1 << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            Search::BlackPassedMask[sq] |= (1 << tsq);
            tsq -= 8;
        }

        if(Board::FilesBrd[SQ120(sq)] > FILE_A) {
            Search::IsolatedMask[sq] |= Search::FileBBMask[Board::FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
                Search::WhitePassedMask[sq] |= (1 << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
                Search::BlackPassedMask[sq] |= (1 << tsq);
                tsq -= 8;
            }        }

        if(Board::FilesBrd[SQ120(sq)] < FILE_H) {
            Search::IsolatedMask[sq] |= Search::FileBBMask[Board::FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
                Search::WhitePassedMask[sq] |= (1 << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
                Search::BlackPassedMask[sq] |= (1 << tsq);
                tsq -= 8;
            }
        }
	}
}


// Returns current time.
int Search::GetTimeMs() { 

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

void Search::CheckUp(SearchInfo& info) {
	// .. check if time up, or interrupt from GUI
	if(info.timeset == true && GetTimeMs() > info.stoptime)
		info.stopped = true;

    UCI::ReadInput(info);
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
	info.stopped = false;
	info.nodes = 0;
	info.fh = 0;
	info.fhf = 0;
}

// Reduces horizon effect
int Search::Quiescence(int alpha, int beta, Board& b, SearchInfo& info) {
    
    assert(b.checkBoard());

    if((info.nodes & 2047) == 0)
    {
    	CheckUp(info);
    }


    info.nodes++;

    // position is draw.
	if((isRepetition(b) || b.m_fiftyMove >= 100) && b.m_ply != 0) {
		return 0;
	}

    // If we've searched to deep
	if(b.m_ply > MAXDEPTH - 1) {
		return EvalPosition(b);
	}

    int Score = EvalPosition(b);

    if(Score >= beta)
    	return beta;

    if(Score > alpha)
       alpha = Score;

    // Generate all captures for board
	MoveList list;
	list.genAllCaps(b);

    int Legal = 0;   // Incremented on legal moves
	int OldAlpha = alpha; // Record what alpha is before loop
	int BestMove = 0;     // Best move found
    Score = 0;
    int PvMove = b.m_pvTable.ProbePvTable(b);

    // Loop over all moves in move list	
    std::vector<Move>::iterator itr;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
			
		/*
		   Pick move according to capture score,
		   improved move ordering makes search more efficient.
	   	*/
		PickNextMove(itr, list);	
		
		// Not legal move
        if ( !MoveGen::makeMove(b,itr->m_move))  {
            continue;
        }
        
		Legal++;

		// Nega Max, flip bounds for opposition colours
		Score = -Quiescence( -beta, -alpha, b, info);	
      
		// Take back move	
        MoveGen::takeMove(b);
		
        if(info.stopped == true)
        	return 0;

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

    // We improved alpha, store best move for position
	if(alpha != OldAlpha) {
		b.m_pvTable.StorePvMove(b, BestMove);
	}

	return alpha;
}

/* Alpa beta search, returns board score
   Alpha - best possible evaluation for maximizer
   Beta - best possile evluation for minimizer
   Beta cutoff - maximizer won't choose option since minimizer has
                 option to reduce score below alpha
*/
int Search::AlphaBeta(int alpha, int beta, int depth, Board& b, SearchInfo& info, bool DoNull) {
 
    #ifndef NDEBUG
    Log* log = Log::getInstance();
    #endif
	
	assert(b.checkBoard()); 
	

	if(depth == 0) {
       return Quiescence(alpha, beta, b, info);
	}
	
	if((info.nodes & 2047) == 0)
    {
    	CheckUp(info);
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
	
	// Increase search depth if in check
	Colour changeSide = static_cast<Colour>(b.m_side^1);
    bool inCheck = b.isSquareAttacked(b.m_kingSq[b.m_side],changeSide);
    if(inCheck)
    	depth++;

    // Null move pruning
    int Score = 0;

	if( DoNull && !inCheck && b.m_ply && (b.m_majPce[b.m_side] > 0) && depth >= 4) {
		MoveGen::makeNullMove(b);
		Score = -AlphaBeta( -beta, -beta + 1, depth-4, b, info, false);
		MoveGen::takeNullMove(b);
		if(info.stopped == true) {
			return 0;
		}

		if (Score >= beta) {
		
			return beta;
		}
	}

	// Generate all moves for board
	MoveList list;
	list.genAllMoves(b);
      
	int Legal = 0;   // Incremented on legal moves

	int OldAlpha = alpha; // Record what alpha is before loop
	int BestMove = 0;     // Best move found
    Score = 0;

    int PvMove = b.m_pvTable.ProbePvTable(b);
    std::vector<Move>::iterator itr;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
       if(itr->m_move == PvMove){
       	  itr->m_score = 2000000;
       	  break;
       }
    }

    // Loop over all moves in move list	
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
			
		/*
		   Pick move according to capture score,
		   improved move ordering makes search more efficient.
	   	*/
		PickNextMove(itr, list);	
		
		// Not legal move
        if ( !MoveGen::makeMove(b,itr->m_move))  {
            continue;
        }
        
		Legal++;

		// Nega Max, flip bounds for opposition colours
		Score = -AlphaBeta( -beta, -alpha, depth-1, b, info, true);	
      
        #ifndef NDEBUG
        char str[50];
        if(depth == 1)
            sprintf(str,"%s node %ld, Score %d, alpha %d, beta %d\n",itr->moveString().c_str(),info.nodes,Score,alpha,beta);
        else if(depth == 2)
        	sprintf(str,"	%s node %ld, Score %d, alpha %d, beta %d\n",itr->moveString().c_str(),info.nodes,Score,alpha,beta);
        else if (depth == 3)
        	sprintf(str,"		%s node %ld, Score %d, alpha %d, beta %d\n",itr->moveString().c_str(),info.nodes,Score,alpha,beta);
        else
             sprintf(str,"			%s node %ld, Score %d, alpha %d, beta %d\n",itr->moveString().c_str(),info.nodes,Score,alpha,beta);


        log->writeLine(str);
        #endif
       
		// Take back move	
        MoveGen::takeMove(b);
		
        if(info.stopped == true)
        	return 0;

		// Improved on alpha
		if(Score > alpha) {
			// Beta cutoff
			if(Score >= beta) {
				if(Legal==1) {
					info.fhf++;
				}
				info.fh++;	

                if(!(itr->m_move & MFLAGCAP)) // Not a capture move
                {
                   b.m_searchKillers[1][b.m_ply] = b.m_searchKillers[0][b.m_ply];
                   b.m_searchKillers[0][b.m_ply] = itr->m_move;

                }


				return beta;  // return beta
			}
			// New best move
			alpha = Score;
			BestMove = itr->m_move;

            if(!(itr->m_move & MFLAGCAP)){
            	b.m_searchHistory[b.m_board[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
            }

		}		
    }
	
	// No legal moves found, checkmate or draw	
	if(Legal == 0) {
		if(inCheck) { 
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
	
	// Reset for current search
	ClearForSearch(b,info);
	
	#ifndef NDEBUG
    Log* log = Log::getInstance();
    #endif

	// iterative deepening for each depth
	for(int currentDepth = 1; currentDepth <= info.depth; ++currentDepth ) {

		int bestScore = AlphaBeta(-INF, INF, currentDepth, b, info, true);
		
        if(info.stopped == true)
        	break;

		pvMoves = b.m_pvTable.GetPvLine(currentDepth, b); // populate pv array
		int bestMove = b.m_pvTable.m_pvArray[0];          // find best move in current position, at depth 0
		
		#ifndef NDEBUG
         Move m;
        m.m_move = bestMove;
         
        char str[200];

        sprintf(str,"\nDepth:%d score:%d move:%s nodes:%ld ",
			currentDepth,bestScore,m.moveString().c_str(),info.nodes);
			
		sprintf(str,"%spv",str);		
		for(int pvNum = 0; pvNum < pvMoves; ++pvNum) {
			Move m;
            m.m_move = b.m_pvTable.m_pvArray[pvNum];
			sprintf(str,"%s %s",str,m.moveString().c_str());
		}
		sprintf(str,"%s\n",str);   

        log->writeLine(str);
         #endif

		////////////////////////////////////////////
        // Output depends on protocol
        ////////////////////////////////////////////
        if(info.gameMode == UCIMODE) {
			printf("info score cp %d depth %d nodes %ld time %d ",
				bestScore,currentDepth,info.nodes,GetTimeMs()- info.starttime);
		} else if(info.gameMode == XBMODE && info.postThinking == true) {
			printf("%d %d %d %ld ",
				currentDepth,bestScore,(GetTimeMs()-info.starttime)/10,info.nodes);
		} else if(info.postThinking == true) {
			printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
				bestScore,currentDepth,info.nodes,GetTimeMs()-info.starttime);
		}
		if(info.gameMode == UCIMODE || info.postThinking == true) {
			printf("pv");		
			for(int pvNum = 0; pvNum < pvMoves; ++pvNum) {
			    printf(" %s",MoveGen::moveString(b.m_pvTable.m_pvArray[pvNum]).c_str());			
			}
			printf("\n");
		}   
   
		
		//////////////////////////////////////////////	
	}
	
	uint32_t bestMove = b.m_pvTable.m_pvArray[0];
	std::string moveString = MoveGen::moveString(bestMove);
	if(info.gameMode == UCIMODE) {
	    printf("bestmove %s\n",moveString.c_str());
	} else if(info.gameMode == XBMODE) {		
		printf("move %s\n",moveString.c_str());
		MoveGen::makeMove(b, bestMove);
	} else {	
		printf("\n\n*** WeeChess makes move %s ***\n\n",moveString.c_str());
		MoveGen::makeMove(b, bestMove);
		b.printBoard();
	}
	
}

// Used Piece tabled to evaulate position
int Search::EvalPosition(const Board& b) {

	Piece pce;
 
    // material score
	int score = b.m_material[WHITE] - b.m_material[BLACK];
	if(!Bitboard::countBits(b.m_pList[wP]) && !Bitboard::countBits(b.m_pList[bP]) && MaterialDraw(b) == true) {
		return 0;
	}

    // White pawns
	bitboard wpBitboard = b.m_pList[wP];
	int numPce = Bitboard::countBits(wpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wpBitboard); 
		score += PawnTable[sq64];

		if( (Search::IsolatedMask[sq64] & wpBitboard) == 0) {
			score += Search::PawnIsolated;
		}
		
		if( (Search::WhitePassedMask[sq64] & b.m_pList[bP]) == 0) {
			score += Search::PawnPassed[Board::RanksBrd[sq64]];
		}
	}	

    // Black pawns
	bitboard bpBitboard = b.m_pList[bP];
	numPce = Bitboard::countBits(bpBitboard);
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&bpBitboard); 
		score -= PawnTable[Mirror64[sq64]];

		if( (Search::IsolatedMask[sq64] & bpBitboard) == 0) {
			score -= Search::PawnIsolated;
		}
		
		if( (Search::BlackPassedMask[sq64] & b.m_pList[wP]) == 0) {
			score -= Search::PawnPassed[7 - Board::RanksBrd[sq64]];
		}
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
		score += BishopTable[sq64];
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

		assert(Board::FilesBrd[sq64] > 0 && Board::FilesBrd[sq64] < 7);
		
		if(!((b.m_pList[bP] | b.m_pList[wP]) & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score += Search::RookOpenFile;
		} else if(!(b.m_pList[wP] & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score += Search::RookSemiOpenFile;
		}


	}	

    // Black Rook
	bitboard brBitboard = b.m_pList[bR];
	numPce = Bitboard::countBits(brBitboard);	
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&brBitboard); 
		score -= RookTable[Mirror64[sq64]];


		assert(Board::FilesBrd[sq64] > 0 && Board::FilesBrd[sq64] < 7);
		if(!((b.m_pList[bP] | b.m_pList[wP]) & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score -= Search::RookOpenFile;
		} else if(!(b.m_pList[bP] & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score -= Search::RookSemiOpenFile;
		}
	}	


	// White Queen
	bitboard wqBitboard = b.m_pList[bR];
	numPce = Bitboard::countBits(wqBitboard);		
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&wqBitboard); 
		assert(Board::FilesBrd[sq64] > 0 && Board::FilesBrd[sq64] < 7);

		if(!((b.m_pList[bP] | b.m_pList[wP]) & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score += Search::QueenOpenFile;
		} else if(!(b.m_pList[wP] & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score += Search::QueenSemiOpenFile;
		}
	}	

	// Black Queen
	bitboard bqBitboard = b.m_pList[bQ];
	numPce = Bitboard::countBits(bqBitboard);		
	for(int pceNum = 0; pceNum < numPce; ++pceNum) {
		int sq64 = Bitboard::popBit(&bqBitboard); 
		assert(Board::FilesBrd[sq64] > 0 && Board::FilesBrd[sq64] < 7);

		if(!((b.m_pList[bP] | b.m_pList[wP]) & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score -= Search::QueenOpenFile;
		} else if(!(b.m_pList[bP] & Search::FileBBMask[Board::FilesBrd[sq64]])) {
			score -= Search::QueenSemiOpenFile;
		}
	}	
	
	// White King
    bitboard wkBitboard = b.m_pList[wK];
	int sq64 = Bitboard::popBit(&wkBitboard); 
	
	if( (b.m_material[BLACK] <= ENDGAME_MAT) ) {
		score += KingE[sq64];
	} else {
		score += KingO[sq64];
	}
	
	// Black King
	bitboard bkBitboard = b.m_pList[bK];
	sq64 = Bitboard::popBit(&bkBitboard); 
	
	if( (b.m_material[WHITE] <= ENDGAME_MAT) ) {
		score -= KingE[Mirror64[sq64]];
	} else {
		score -= KingO[Mirror64[sq64]];
	}
	
	if(Bitboard::countBits(wbBitboard) >= 2) 
		score += Search::BishopPair;

	if(Bitboard::countBits(bbBitboard)  >= 2) 
		score -= Search::BishopPair;
	
	if(b.m_side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}

bool Search::MaterialDraw(const Board& b)
{

	assert(b.checkBoard());

	const int numbR = Bitboard::countBits(b.m_pList[bR]);
	const int numwR = Bitboard::countBits(b.m_pList[wR]);
	const int numbQ = Bitboard::countBits(b.m_pList[bQ]);
	const int numwQ = Bitboard::countBits(b.m_pList[wQ]);
	const int numwN = Bitboard::countBits(b.m_pList[wN]);
	const int numbN = Bitboard::countBits(b.m_pList[bN]);
	const int numwB = Bitboard::countBits(b.m_pList[wB]);
	const int numbB = Bitboard::countBits(b.m_pList[bB]);

	
    if (numwR != 0 && numbR != 0 && numwQ != 0 && numbQ != 0) {
	    if (numbB != 0 && numwB != 0) {
	        if (numwN < 3 && numbN < 3) {  
	        	return true; 
	        }
	    } else if (numwN != 0 && numbN != 0) {
	        if (abs(numwB - numbB) < 2) { 
	        	return true; }
	    } else if ( (numwN < 3 && numwB !=0) || (numwB == 1 && numwN !=0 )) {
	        if ( (numbN < 3 && numbB !=0) || (numbB == 1 && numbN != 0)) { 
	        	return true; 
	        }
	    }
	} else if (numwQ != 0 && numbQ != 0) {
        if ( numwR == 1 && numbR == 1) {
            if (numwN + numwB < 2 && numbN + numbB < 2)	{ 
            	return true; 
            }
        } else if ( numwR == 1 && numbR !=0 ) {
            if ( (numwN + numwB == 0 && numbB == 1) || (numbN + numbB == 2)) { 
            	return true; 
            }
        } else if (numbR == 1 && numwR !=0 ) {
            if (numbN + numbB == 0 && ((numwN + numbN == 1) || (numwN + numwB == 2))) { 
            	return true; 
            }
        }
    }
  return false;
}

// Finds the best scoring move in the move list and swaps with current move
void Search::PickNextMove(std::vector<Move>::iterator move, MoveList& list) {


    std::vector<Move>::iterator result = std::max_element(move, list.m_move_vec.end(), 
    	                                 [](Move a, Move b){
                                             return a.m_score < b.m_score;
    	                                  });

	std::iter_swap(result, move);
}