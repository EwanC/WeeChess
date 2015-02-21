#include "move.hpp"
#include "board.hpp"

#include <iostream>
#include <cassert>


 // Directions that each piece can move in
 static const int PceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

// Castle Permission Array 
// All initally set to 0xF apart from a1,e1,h1,a8,e8, and h8
// Every time a piece is moved the permission &= castlePerm[form/to]
static const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

// How many directinons each piece has
static const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};



 Move::Move():m_move(0),m_score(0)
 {

 }

 std::string Move::moveString() const
 {
    std::string move_str;
	
	int ff = Board::FilesBrd[FROMSQ(m_move)];
	int rf = Board::RanksBrd[FROMSQ(m_move)];
	int ft = Board::FilesBrd[TOSQ(m_move)];
	int rt = Board::RanksBrd[TOSQ(m_move)];
	
	move_str.push_back('a'+ff);
    move_str.push_back('1'+rf);
	move_str.push_back('a'+ft);
	move_str.push_back('1'+rt);
	
	int promoted = PROMOTED(m_move);
	
	if(promoted) {
		char pchar = 'q';
		if(Board::PieceKnight[promoted]) {
			pchar = 'n';
		} else if(Board::PieceRookQueen[promoted] && !Board::PieceBishopQueen[promoted])  {
			pchar = 'r';
		} else if(!Board::PieceRookQueen[promoted] && Board::PieceBishopQueen[promoted])  {
			pchar = 'b';
		}
		
    	move_str.push_back(pchar);
	} 
	
	return move_str;
 }

 /*
  Movegen -> loop over all pieces 
             -> slider loop in each direction add move
               ->addmove list->moves.push_back(move)

 */

void MoveList::genAllMoves(const Board& b) {
   
    static const Piece SlidePce[2][3] = {
       {wB, wR, wQ},{bB, bR, bQ}
    };

    static const Piece NonSlidePce[2][2] = {
       {wN, wK},{bN, bK}
    };

	if(b.m_side == WHITE) {
		bitboard wpBitboard = b.m_pList[wP];
		for(int pceNum = 0; pceNum < Bitboard::countBits(wpBitboard); ++pceNum) {
			int sq = Bitboard::popBit(&wpBitboard);
			assert(!SQOFFBOARD(sq));
			
			if(b.m_board[sq + 10] == EMPTY) {
				addPawnMove<WHITE>(b, sq, sq+10);
				if(Board::RanksBrd[sq] == RANK_2 && b.m_board[sq + 20] == EMPTY) {
					addQuietMove(b, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS));
				}
			} 
			
			if(!SQOFFBOARD(sq + 9) && Board::PieceCol[b.m_board[sq + 9]] == BLACK) {
				addPawnCapMove<WHITE>(b, sq, sq+9, b.m_board[sq + 9]);
			}  
			if(!SQOFFBOARD(sq + 11) && Board::PieceCol[b.m_board[sq + 11]] == BLACK) {
				addPawnCapMove<WHITE>(b, sq, sq+11, b.m_board[sq + 11]);
			} 
			
			if(sq + 9 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP));
			} 
			if(sq + 11 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP));
			}
		}
		
		// castling
		if(b.m_castling & WKCA) {
			if(b.m_board[F1] == EMPTY && b.m_board[G1] == EMPTY) {
				if(!b.isSquareAttacked(E1,BLACK) && !b.isSquareAttacked(F1,BLACK) ) {
					addQuietMove(b, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}
		
		if(b.m_castling & WQCA) {
			if(b.m_board[D1] == EMPTY && b.m_board[C1] == EMPTY && b.m_board[B1] == EMPTY) {
				if(!b.isSquareAttacked(E1,BLACK) && !b.isSquareAttacked(D1,BLACK) ) {
					addQuietMove(b, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}
		
	} else {
		
		bitboard bpBitboard = b.m_pList[bP];
		for(int pceNum = 0; pceNum < Bitboard::countBits(bpBitboard); ++pceNum) {
			int sq = Bitboard::popBit(&bpBitboard);
			assert(!SQOFFBOARD(sq));
			
			if(b.m_board[sq - 10] == EMPTY) {
				addPawnMove<BLACK>(b, sq, sq-10);
				if(Board::RanksBrd[sq] == RANK_7 && b.m_board[sq - 20] == EMPTY) {
					addQuietMove(b, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS));
				}
			} 
			
			if(!SQOFFBOARD(sq - 9) && Board::PieceCol[b.m_board[sq - 9]] == WHITE) {
				addPawnCapMove<BLACK>(b, sq, sq-9, b.m_board[sq - 9]);
			} 
			
			if(!SQOFFBOARD(sq - 11) && Board::PieceCol[b.m_board[sq - 11]] == WHITE) {
				addPawnCapMove<BLACK>(b, sq, sq-11, b.m_board[sq - 11]);
			} 
			
			if(sq - 9 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP));
			} 
			if(sq - 11 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP));
			}
		}


		// castling
		if(b.m_castling &  BKCA) {
			if(b.m_board[F8] == EMPTY && b.m_board[G8] == EMPTY) {
				if(!b.isSquareAttacked(E8,WHITE) && !b.isSquareAttacked(F8,WHITE) ) {
					addQuietMove(b, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}
		
		if(b.m_castling &  BQCA) {
			if(b.m_board[D8] == EMPTY && b.m_board[C8] == EMPTY && b.m_board[B8] == EMPTY) {
				if(!b.isSquareAttacked(E8,WHITE) && !b.isSquareAttacked(D8,WHITE) ) {
					addQuietMove(b, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA));
				}
			}
		}		
	
	}
   
    /* Loop for slide pieces */

 	for(Piece pce : SlidePce[b.m_side]) {
		assert(pce >= wP && pce <= bK);		
		bitboard bb = b.m_pList[static_cast<int>(pce)];
		for(int pceNum = 0; pceNum < b.m_pceNum[pce]; ++pceNum) {
			int sq = Bitboard::popBit(&bb);
			assert(!SQOFFBOARD(sq));
			
			for(int index = 0; index < NumDir[pce]; ++index) {
				int dir = PceDir[pce][index];
				int t_sq = sq + dir;
				
				while(!SQOFFBOARD(t_sq)) {				
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if(b.m_board[t_sq] != EMPTY) {
						if( Board::PieceCol[b.m_board[t_sq]] == (b.m_side ^ 1)) {
							addCaptureMove(b, MOVE(sq, t_sq, b.m_board[t_sq], EMPTY, 0));
						}
						break;
					}	
					addQuietMove(b, MOVE(sq, t_sq, EMPTY, EMPTY, 0));
					t_sq += dir;
				}
			}
		}
	}

	/* Loop for non slide */
 	for(Piece pce : NonSlidePce[b.m_side]) {
		assert(pce >= wP && pce <= bK);		
	    bitboard bb = b.m_pList[static_cast<int>(pce)];
		
		for(int pceNum = 0; pceNum < b.m_pceNum[pce]; ++pceNum) {
			int sq = Bitboard::popBit(&bb);
			assert(!SQOFFBOARD(sq));
			
			for(int index = 0; index < NumDir[pce]; ++index) {
				int dir = PceDir[pce][index];
				int t_sq = sq + dir;
				
				if(SQOFFBOARD(t_sq)) {				    
					continue;
				}
				
				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if(b.m_board[t_sq] != EMPTY) {
					if( Board::PieceCol[b.m_board[t_sq]] == (b.m_side ^ 1)) {
						addCaptureMove(b, MOVE(sq, t_sq, b.m_board[t_sq], EMPTY, 0));
					}
					continue;
				}	
				addQuietMove(b, MOVE(sq, t_sq, EMPTY, EMPTY, 0));
			}
		}
	}

}

template<Colour colour>
void MoveList::addPawnMove( const Board& b, const int from, const int to ) {

	assert(!SQOFFBOARD(from));
	assert(!SQOFFBOARD(to));
	
    if(Board::RanksBrd[from] == RANK_7 && colour == WHITE) {
	    addQuietMove(b, MOVE(from,to,EMPTY,wQ,0));
		addQuietMove(b, MOVE(from,to,EMPTY,wR,0));
		addQuietMove(b, MOVE(from,to,EMPTY,wB,0));
		addQuietMove(b, MOVE(from,to,EMPTY,wN,0));
	} 
    else if(Board::RanksBrd[from] == RANK_2 && colour == BLACK) {
 	    addQuietMove(b, MOVE(from,to,EMPTY,bQ,0));
 		addQuietMove(b, MOVE(from,to,EMPTY,bR,0));
 	    addQuietMove(b, MOVE(from,to,EMPTY,bB,0));
 		addQuietMove(b, MOVE(from,to,EMPTY,bN,0));
 	} else {
 	    addQuietMove(b, MOVE(from,to,EMPTY,EMPTY,0));
 	}
    
}

template<Colour colour>
void MoveList::addPawnCapMove( const Board& b, const int from, const int to, const Piece cap) {	
	
	assert(cap >= EMPTY && cap <= bK);
	assert(!SQOFFBOARD(from));
	assert(!SQOFFBOARD(to));

    
	if(Board::RanksBrd[from] == RANK_7 && colour == WHITE) {
		addCaptureMove(b, MOVE(from,to,cap,wQ,0));
		addCaptureMove(b, MOVE(from,to,cap,wR,0));
		addCaptureMove(b, MOVE(from,to,cap,wB,0));
		addCaptureMove(b, MOVE(from,to,cap,wN,0));
	}
    else if(Board::RanksBrd[from] == RANK_2 && colour == BLACK) {
		addCaptureMove(b, MOVE(from,to,cap,bQ,0));
		addCaptureMove(b, MOVE(from,to,cap,bR,0));
		addCaptureMove(b, MOVE(from,to,cap,bB,0));
		addCaptureMove(b, MOVE(from,to,cap,bN,0));
    } else {
		addCaptureMove(b, MOVE(from,to,cap,EMPTY,0));
	} 
	
}

void MoveList::addQuietMove(const Board& b, uint32_t move) {
	Move m;
	m.m_move = move;
	m.m_score = 0;

	m_move_vec.push_back(m);
}

void MoveList::addCaptureMove(const Board& b, uint32_t move) {
	Move m;
	m.m_move = move;
	m.m_score = 0;

	m_move_vec.push_back(m);
}

void MoveList::addEnPassMove(const Board& b, uint32_t move) {
	Move m;
	m.m_move = move;
	m.m_score = 0;

	m_move_vec.push_back(m);
}

void MoveList::printList() const
{
   std::vector<Move>::const_iterator itr;
   unsigned int count = 0;
   for(itr = m_move_vec.begin(); itr != m_move_vec.end(); itr++) {
   	  std::cout << "Move " << count << ": "<<std::hex<<itr->m_move <<std::dec << " score: "<<itr->m_score<<std::endl;
   }


}

void ClearPiece(const int sq, Board& b) {

	assert(!SQOFFBOARD(sq));
	
    Piece pce = b.m_board[sq];
	
    assert(pce >= wP && pce <= bK);
	
	Colour colour = Board::PieceCol[pce];
	
    HASH_PCE(pce,sq);
	
	b.m_board[sq] = EMPTY;
    b.m_material[colour] -= Board::PieceVal[pce];
	b.m_pceNum[pce]--;		

	if(pce != wP && pce != bP) {
		if(Board::PieceMaj[pce]) {
			b.m_majPce[colour]--;
		} else {
			b.m_minPce[colour]--;
		}
	} 
	
	CLRBIT(b.m_pList[pce],SQ64(sq));
	
}

void AddPiece(const int sq, Board& b, const Piece pce) {

    assert(pce >= wP && pce <= bK);
	assert(!SQOFFBOARD(sq));
	
	Colour colour = Board::PieceCol[pce];

    HASH_PCE(pce,sq);
	
	b.m_board[sq] = pce;

    if(pce != wP && pce != bP) {
		if(Board::PieceMaj[pce]) {
			b.m_majPce[colour]++;
		} else {
			b.m_minPce[colour]++;
		}
	} 

	SETBIT(b.m_pList[pce],SQ64(sq));

	b.m_material[colour] += Board::PieceVal[pce];
	
}

void MovePiece(const int from, const int to, Board& b) {

	assert(!SQOFFBOARD(from));
	assert(!SQOFFBOARD(to));
	
	Piece pce = b.m_board[from];	
	Colour col = Board::PieceCol[pce];
	
	HASH_PCE(pce,from);
	b.m_board[from] = EMPTY;
	
	HASH_PCE(pce,to);
	b.m_board[to] = pce;
	
	   
	CLRBIT(b.m_pList[pce],SQ64(from));
	SETBIT(b.m_pList[pce],SQ64(to));

}

bool MakeMove(Board& b, int move) {

	assert(b.checkBoard());
	
	int from = FROMSQ(move);
    int to = TOSQ(move);
    Colour side = b.m_side;
	
	assert(!SQOFFBOARD(from));
	assert(!SQOFFBOARD(to));
    assert(side==WHITE || side == BLACK);
    assert(b.m_board[from] >= wP && b.m_board[from] <= bK);
	

	b.m_history[b.m_hisply].posHash = b.m_posHash;
	
	if(move & MFLAGEP) {
        if(side == WHITE) {
            ClearPiece(to-10,b);
        } else {
            ClearPiece(to+10,b);
        }
    } else if (move & MFLAGCA) {
        switch(to) {
            case C1:
                MovePiece(A1, D1, b);
			break;
            case C8:
                MovePiece(A8, D8, b);
			break;
            case G1:
                MovePiece(H1, F1, b);
			break;
            case G8:
                MovePiece(H8, F8, b);
			break;
            default: assert(false && "Make move failed"); break;
        }
    }	
	
	if(b.m_enPas != NO_SQ)
	{
		HASH_EP;
	}

    HASH_CA;
	
	b.m_history[b.m_hisply].move = move;
    b.m_history[b.m_hisply].fiftyMove = b.m_fiftyMove;
    b.m_history[b.m_hisply].enPas = b.m_enPas;
    b.m_history[b.m_hisply].castlingPerm = b.m_castling;

    b.m_castling &= CastlePerm[from];
    b.m_castling &= CastlePerm[to];
    b.m_enPas = NO_SQ;

	HASH_CA;
	
	int captured = CAPTURED(move);
    b.m_fiftyMove++;
	
	if(captured != EMPTY) {
        assert(captured >= wP && captured <= bK);
        ClearPiece(to, b);
        b.m_fiftyMove = 0;
    }
	
	b.m_hisply++;
	b.m_ply++;
	
	if(b.m_board[from] == wP || b.m_board[from] == bP) {
        b.m_fiftyMove = 0;
        if(move & MFLAGPS) {
            if(side==WHITE) {
                b.m_enPas=static_cast<Square>(from+10);
                assert(Board::RanksBrd[b.m_enPas]==RANK_3);
            } else {
                b.m_enPas=static_cast<Square>(from-10);
                assert(Board::RanksBrd[b.m_enPas]==RANK_6);
            }
            HASH_EP;
        }
    }
	
	MovePiece(from, to, b);
	
	Piece prPce = static_cast<Piece>(PROMOTED(move));
    if(prPce != EMPTY)   {
        assert(prPce != bP && b.m_board[from] > wP && b.m_board[from] <= bK);
        ClearPiece(to, b);
        AddPiece(to, b, prPce);
    }
	
	if(Board::PieceKing[b.m_board[to]]) {
        b.m_kingSq[b.m_side] = static_cast<Square>(to);
    }
	
	b.m_side = static_cast<Colour>(b.m_side ^ 1);
    HASH_SIDE;

    assert(b.checkBoard());
	
		
	if(b.isSquareAttacked(b.m_kingSq[side],b.m_side))  {
        TakeMove(b);
        return false;
    }
	
	return true;
	
}

void TakeMove(Board& b) {
	
	assert(b.checkBoard());
	
	b.m_hisply--;
    b.m_ply--;
	
    int move = b.m_history[b.m_hisply].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);	
	
	assert(!SQOFFBOARD(from));
	assert(!SQOFFBOARD(to));
	
	if(b.m_enPas != NO_SQ) 
	{
		HASH_EP;
	}
    HASH_CA;

    b.m_castling = b.m_history[b.m_hisply].castlingPerm;
    b.m_fiftyMove = b.m_history[b.m_hisply].fiftyMove;
    b.m_enPas = b.m_history[b.m_hisply].enPas;

    if(b.m_enPas != NO_SQ) 
    {
    	HASH_EP;
    }

    HASH_CA;

    b.m_side = static_cast<Colour>(b.m_side ^ 1);
    HASH_SIDE;
	
	if(MFLAGEP & move) {
        if(b.m_side == WHITE) {
            AddPiece(to-10, b, bP);
        } else {
            AddPiece(to+10, b, wP);
        }
    } else if(MFLAGCA & move) {
        switch(to) {
            case C1: MovePiece(D1, A1, b); break;
            case C8: MovePiece(D8, A8, b); break;
            case G1: MovePiece(F1, H1, b); break;
            case G8: MovePiece(F8, H8, b); break;
            default: assert(false && "Take move failed"); break;
        }
    }
	
	MovePiece(to, from, b);
	
	if(Board::PieceKing[b.m_board[from]]) {
        b.m_kingSq[b.m_side] = static_cast<Square>(from);
    }
	
	Piece captured = static_cast<Piece>(CAPTURED(move));
    if(captured != EMPTY) {
        assert(captured >= wP && captured <= bK);
        AddPiece(to, b, captured);
    }
	
	if(PROMOTED(move) != EMPTY)   {
		Piece prPce = static_cast<Piece>(PROMOTED(move));
        assert(prPce != bP && b.m_board[from] > wP && b.m_board[from] <= bK);

        ClearPiece(from, b);
        AddPiece(from, b, (Board::PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
    }
	
    assert(b.checkBoard());

}