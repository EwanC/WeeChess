#include "move.hpp"
#include "board.hpp"

#include <iostream>
#include <cassert>


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
				addPawnCapMove<BLACK>(b, sq, sq-11, b.m_board[sq - 1]);
			} 
			
			if(sq - 9 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP));
			} 
			if(sq - 11 == b.m_enPas) {
				addCaptureMove(b, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP));
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