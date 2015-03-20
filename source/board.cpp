#include "board.hpp"
#include "search.hpp"
#include <cassert>
#include <ctime>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <string>

// Generate random unsigned 64 bit int
#define RAND_64 ((uint64_t)rand() | \
			     (uint64_t)rand() << 15 | \
				 (uint64_t)rand() << 30 | \
				 (uint64_t)rand() << 45 | \
				 ((uint64_t)rand() & 0xf) << 60 )  

namespace Bitboard{


const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

// returns the index of least signifcant bit and sets it to zero
int popBit(bitboard *bb) {
  uint64_t b = *bb ^ (*bb - 1);
  uint32_t fold = (uint32_t) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

// Counts the number of bits in the bitboard
int countBits(bitboard b) {
  int r;
  for(r = 0; b; r++, b &= b - 1);
  return r;
}

// Prints bitboard to standard out
void printBitboard(bitboard bb) {

	uint64_t shiftMe = 1;
	
	std::cout<<std::endl;
	for(int rank = RANK_8; rank >= RANK_1; --rank) {
		for(int file = FILE_A; file <= FILE_H; ++file) {
			int sq = FR2SQ(file,rank);	// 120 based		
			int sq64 = SQ64(sq);        // 64 based
			
			if((shiftMe << sq64) & bb) 
				std::cout<<'X';
			else 
				std::cout<<'-';
				
		}
	   std::cout<<std::endl;
	}  
	std::cout<<std::endl<<std::endl;
}


} //namespace

/*
   Static data
 */
// Strings for pretty printing
const char* Board::PceChar = ".PNBRQKpnbrqk";
const char* Board::SideChar = "wb-";
const char* Board::RankChar = "12345678";
const char* Board::FileChar = "abcdefgh";

// Piece Directions
const int Board::KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int Board::RkDir[4] = { -1, -10,	1, 10 };
const int Board::BiDir[4] = { -9, -11, 11, 9 };
const int Board::KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };

// Major Pieces, i.e. Rook, Queen, King
const bool Board::PieceMaj[13] = { false, false, false, false, true, true, true, false, false, false, true, true, true };

// Min Pieces, i.e Bishop, Knight
const bool Board::PieceMin[13] = { false, false, true, true, false, false, false, false, true, true, false, false, false };

// Piece Values
const unsigned int Board::PieceVal[13]= { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000  };

//Piece Colours
const Colour Board::PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
	
// Piece Types
const bool Board::PieceKnight[13] = { false, false, true, false, false, false, false, false, true, false, false, false, false };
const bool Board::PieceKing[13] = { false, false, false, false, false, false, true, false, false, false, false, false, true };
const bool Board::PieceRookQueen[13] = { false, false, false, false, true, true, false, false, false, false, true, true, false };
const bool Board::PieceBishopQueen[13] = { false, false, false, true, false, true, false, false, false, true, false, true, false };
const bool Board::PieceSlides[13] = { false, false, false, true, true, true, false, false, false, true, true, true, false };

int Board::FilesBrd[TOTAL_SQUARES] = {0}; // file index of each square
int Board::RanksBrd[TOTAL_SQUARES] = {0}; // rank index of each square

Board::Board()
{
    seedRandNums(); // Seed data for random number gen

    // Setup starting board position
    clearBoard();
}

// Initalizes static members
void Board::initStaticMembers()
{
	for(int index = 0; index < TOTAL_SQUARES; ++index) {
		Board::FilesBrd[index] = OFFBOARD;
		Board::RanksBrd[index] = OFFBOARD;
	}
	
	for(int rank = RANK_1; rank <= RANK_8; ++rank) {
		for(int file = FILE_A; file <= FILE_H; ++file) {
			int sq = FR2SQ(file,rank);
			Board::FilesBrd[sq] = file;
			Board::RanksBrd[sq] = rank;
		}
	}
}

void Board::resetBoard() 
{

	for(int index = 0; index < 64; ++index) {
		m_board[SQ120(index)] = EMPTY;
	}

	for(int index = 0; index < 2; ++index) {
		m_majPce[index] = 0;
		m_minPce[index] = 0;
		m_material[index] = 0;
	}

	m_kingSq[WHITE] = m_kingSq[BLACK] = NO_SQ;

	m_side = BOTH;
	m_enPas = NO_SQ;
	m_fiftyMove = 0;

	m_ply = 0;
	m_hisply = 0;

	m_castling = 0;

	m_posHash = 0;

}

void Board::MirrorBoard()
{
 
    Piece tempPiecesArray[64];
    Colour tempSide = static_cast<Colour>(m_side^1);

	Piece SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;


    if (m_castling & WKCA) tempCastlePerm |= BKCA;
    if (m_castling & WQCA) tempCastlePerm |= BQCA;

    if (m_castling & BKCA) tempCastlePerm |= WKCA;
    if (m_castling & BQCA) tempCastlePerm |= WQCA;

	if (m_enPas != NO_SQ)  {
        tempEnPas = SQ120(Mirror64[SQ64(m_enPas)]);
    }

    for (int sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = m_board[SQ120(Mirror64[sq])];
    }

    resetBoard();

	for (int sq = 0; sq < 64; sq++) {
        Piece tp = SwapPiece[tempPiecesArray[sq]];
        m_board[SQ120(sq)] = tp;
    }

	m_side = tempSide;
    m_castling = tempCastlePerm;
    m_enPas = static_cast<Square>(tempEnPas);

    m_posHash = genHashKey();

	updateListsMaterial();

    assert(checkBoard()); 


}

// update material member data
void Board::updateListsMaterial()
{
	
	for(int index = 0; index < TOTAL_SQUARES; ++index) {
		int sq = index;
		Piece piece = m_board[index];
		if(piece!= EMPTY) {
			Colour colour = PieceCol[piece];
			
		    if( Board::PieceMin[piece] == true) m_minPce[colour]++;
		    if( Board::PieceMaj[piece] == true) m_majPce[colour]++;
			
			m_material[colour] += Board::PieceVal[piece];
			
			SETBIT(m_pList[piece], SQ64(sq));
			SETBIT(m_pList[Piece::EMPTY], SQ64(sq));
			
			if(piece==wK) m_kingSq[WHITE] = static_cast<Square>(sq);
			if(piece==bK) m_kingSq[BLACK] = static_cast<Square>(sq);	
			
		}
	}

}

// Sets up board based on FEN string
bool Board::parseFen(std::string fen_str) 
{

	char* fen = &fen_str[0u];
	assert(fen!=NULL && "FEN is null");
	
	int rank = RANK_8;
    int file = FILE_A;
    Piece  piece;
    uint32_t count = 0;
	
	clearBoard();
	
	while ((rank >= RANK_1) && *fen) {
	    count = 1;
		switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;              

            default:
                std::cout<<"FEN error \n";
                return false;
        }		
		
		for (uint32_t i = 0; i < count; i++) {			
            int sq64 = rank * 8 + file;
			int sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                m_board[sq120] = piece;
            }
			file++;
        }
		fen++;
	}
	
	assert(*fen == 'w' || *fen == 'b' && "invalid Fen");
	
	m_side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;
	
	for (int i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }		
		switch(*fen) {
			case 'K': m_castling |= WKCA; break;
			case 'Q': m_castling |= WQCA; break;
			case 'k': m_castling |= BKCA; break;
			case 'q': m_castling |= BQCA; break;
			default:	     break;
        }
		fen++;
	}
	fen++;
	
	assert(m_castling>=0 && m_castling <= 15 && "Fen has invalid castling");
	
	if (*fen != '-') {        
		file = fen[0] - 'a';
		rank = fen[1] - '1';
		
		assert(file>=FILE_A && file <= FILE_H && "Fen invalid file");
		assert(rank>=RANK_1 && rank <= RANK_8 && "Fen invalid rank");
		
		m_enPas = static_cast<Square>(FR2SQ(file,rank));	
    }
	
	m_posHash= genHashKey(); 
    updateListsMaterial();

	return true;
}

// Returns true if given square is attacked by colour paramter
bool Board::sqAttacked(const int sq, Colour side)
{
  	
	// pawns
	if(side == WHITE) {
		if(m_board[sq-11] == wP || m_board[sq-9] == wP) {
			return true;
		}
	} else {
		if(m_board[sq+11] == bP || m_board[sq+9] == bP) {
			return true;
		}	
	}
	
	// knights
	for(int index = 0; index < 8; ++index) {		
		Piece pce = m_board[sq + Board::KnDir[index]];
		if(Board::PieceKnight[pce] && Board::PieceCol[pce]==side) {
			return true;
		}
	}
	
	// rooks, queens
	for(int index = 0; index < 4; ++index) {		
		int dir = Board::RkDir[index];
		int t_sq = sq + dir;
		Piece pce = m_board[t_sq];
		while(!isOffboard(t_sq)) { 
			if(pce != EMPTY) {
				if(Board::PieceRookQueen[pce] && Board::PieceCol[pce] == side) {
					return true;
				}
				break;
			}
			t_sq += dir;
			pce = m_board[t_sq];
		}
	}
	
	// bishops, queens
	for(int index = 0; index < 4; ++index) {		
		int dir = Board::BiDir[index];
		int t_sq = sq + dir;
		Piece pce = m_board[t_sq];
		while(!isOffboard(t_sq)) {
			if(pce != EMPTY) {
				if(Board::PieceBishopQueen[pce] && Board::PieceCol[pce] == side) {
					return true;
				}
				break;
			}
			t_sq += dir;
			pce = m_board[t_sq];
		}
	}
	
	// kings
	for(int index = 0; index < 8; ++index) {		
		Piece pce = m_board[sq + Board::KiDir[index]];
		if(Board::PieceKing[pce] && Board::PieceCol[pce]==side) {
			return true;
		}
	}
	
	return false;

}

inline bool Board::isOffboard(const int sq120) const
{
  return (sq120 < 21 || sq120 > 98 || sq120 % 10 == 0 || (sq120 + 1) == 0);
}

// Clears all member data
void Board::clearBoard()
{
	
	for(int index = 0; index < TOTAL_SQUARES; ++index) {
		m_board[index] = EMPTY;
	}
	
	for(int index = 0; index < 2; ++index) {
		m_majPce[index] = 0;
		m_minPce[index] = 0;
	    m_material[index] = 0;
	}
	
	for(int index = 0; index < 13; ++index) {
		m_pList[index] = 0;
	}
	
	m_kingSq[WHITE] = m_kingSq[BLACK] = NO_SQ;
	m_side = BOTH;
	m_enPas = NO_SQ;
	m_fiftyMove = 0;
	
	m_ply = 0;
	m_hisply = 0;
	
	m_castling = 0;
	
	m_posHash = 0;


}

// Prints board to stdout
void Board::printBoard() const 
{
	
	
	std::cout <<"\nGame Board:\n\n";
	
	for(int rank = RANK_8; rank >= RANK_1; rank--) {
		std::cout<< rank+1 << " ";
		for(int file = FILE_A; file <= FILE_H; file++) {
			int sq = FR2SQ(file,rank);
			int piece = m_board[sq];
			std::cout<<std::setw(3)<<PceChar[piece];
		}
		std::cout <<std::endl;
	}
	
    std::cout <<std::endl<< "  ";
	for(int file = FILE_A; file <= FILE_H; file++) {
	std::cout<<std::setw(3)<<static_cast<char>('a'+file);	
	}

	std::cout <<std::endl;
	std::cout <<"side:"<<SideChar[m_side]<<std::endl;
	if(m_enPas == NO_SQ )
	    std::cout <<"enPas:"<<'-' <<std::endl;
    else
    	std::cout << static_cast<int>(m_enPas)<<std::endl;
	std::cout <<"castle:";
	std::cout << (m_castling & WKCA ? 'K' : '-');
	std::cout << (m_castling & WQCA ? 'Q' : '-');
	std::cout << (m_castling & BKCA ? 'k' : '-');
	std::cout << (m_castling & BQCA ? 'q' : '-')<<std::endl;
			
	std::cout<<"PosKey:"<<std::hex<<m_posHash<<std::endl;
}

// Seed rand() and set keys for hashing
void Board::seedRandNums()
{
    srand(time(NULL));
   
	for(int index = 0; index < 13; ++index) {
		for(int index2 = 0; index2 < 120; ++index2) {
			pieceKeys[index][index2] = RAND_64;
		}
	}

	sideKey = RAND_64;

	for(int index = 0; index < 16; ++index) {
		castleKeys[index] = RAND_64;
	}
 
}


// Generate zorbist hash of position
uint64_t Board::genHashKey() const
{
	uint64_t finalKey = 0;
	
	for(int sq = 0; sq < TOTAL_SQUARES; ++sq) {
		Piece piece = m_board[sq];
		if( piece!=EMPTY ) {
			assert(piece>=wP && piece<=bK && "Invalid Piece");
			finalKey ^= pieceKeys[piece][sq];
		}		
	}
	
	if(m_side == WHITE) {
	   finalKey ^= sideKey;
	}
		
	if(m_enPas != NO_SQ) {
		assert(m_enPas>=0 && m_enPas<TOTAL_SQUARES && "Invalid En Passant square");
		finalKey ^= pieceKeys[EMPTY][m_enPas];
	}
	
	assert(m_castling >=0 && m_castling<= 15 && "Invalid Casting permission");
	
	finalKey ^= castleKeys[m_castling];
	
	return finalKey;
}

// Returns true if board has a valid position
bool Board::checkBoard() const
{

	int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int t_majPce[2] = { 0, 0};
	int t_minPce[2] = { 0, 0};
	int t_material[2] = { 0, 0};
	
	bitboard t_pawns[3] = {0ULL, 0ULL, 0ULL};
	

	t_pawns[WHITE] = m_pList[Piece::wP];
	t_pawns[BLACK] = m_pList[Piece::bP];
	t_pawns[BOTH] = m_pList[Piece::wP] | m_pList[Piece::bP];
	

	// check piece lists
	for(int t_piece = wP; t_piece <= bK; ++t_piece) {
		bitboard bbCopy = m_pList[t_piece];
		int pieces = Bitboard::countBits(bbCopy);

		for(int t_pce_num = 0; t_pce_num < pieces; ++t_pce_num) {
			int sq120 = SQ120(Bitboard::popBit(&bbCopy));
			assert(m_board[sq120]==t_piece);
		}	
	}
	
	// check piece count and other counters	
	for(int sq64 = 0; sq64 < 64; ++sq64) {
		int sq120 = SQ120(sq64);
		int t_piece = m_board[sq120];
		t_pceNum[t_piece]++;
		Colour colour = PieceCol[t_piece];
		if( PieceMin[t_piece] == true) t_minPce[colour]++;
		if( PieceMaj[t_piece] == true) t_majPce[colour]++;
		
		t_material[colour] += PieceVal[t_piece];
	}
	
	for(int t_piece = wP; t_piece <= bK; ++t_piece) {
		assert(t_pceNum[t_piece]==Bitboard::countBits(m_pList[t_piece]));	
	}
	

	// check bitboards squares
	while(t_pawns[WHITE]) {
		int sq64 = Bitboard::popBit(&t_pawns[WHITE]);
		assert(m_board[SQ120(sq64)] == wP);
	}
	
	while(t_pawns[BLACK]) {
		int sq64 = Bitboard::popBit(&t_pawns[BLACK]);
		assert(m_board[SQ120(sq64)] == bP);
	}
	
	while(t_pawns[BOTH]) {
		int sq64 = Bitboard::popBit(&t_pawns[BOTH]);
		assert( (m_board[SQ120(sq64)] == bP) || (m_board[SQ120(sq64)] == wP) );
	}
	
	assert(t_material[WHITE]==m_material[WHITE] && t_material[BLACK]==m_material[BLACK]);
	assert(t_minPce[WHITE]==m_minPce[WHITE] && t_minPce[BLACK]==m_minPce[BLACK]);
	assert(t_majPce[WHITE]==m_majPce[WHITE] && t_majPce[BLACK]==m_majPce[BLACK]);
	
	assert(m_side==WHITE || m_side==BLACK);
	assert(genHashKey()==m_posHash);
	
	assert(m_enPas==NO_SQ || ( Board::RanksBrd[m_enPas]==RANK_6 && m_side == WHITE)
		 || ( Board::RanksBrd[m_enPas]==RANK_3 && m_side == BLACK));
	
	assert(m_board[m_kingSq[WHITE]] == wK);
	assert(m_board[m_kingSq[BLACK]] == bK);
		 
	return true;

}

bool Board::isSquareAttacked(const Square& sq, const Colour& side) const
{

    assert(Board::FilesBrd[sq] != OFFBOARD);
    assert(side == WHITE || side == BLACK);
    assert(checkBoard());

    // pawns
    if(side == Colour::WHITE) {
        if(m_board[sq-11] == wP || m_board[sq-9] == wP) {
            return true;
        }
    } else {
        if(m_board[sq+11] == bP || m_board[sq+9] == bP) {
            return true;
        }
    }

	// knights
	for(int index = 0; index < 8; ++index) {
		Piece pce = m_board[sq + KnDir[index]];
		if(Board::PieceKnight[pce] && PieceCol[pce]==side) {
			return true;
		}
	}

	// rooks, queens
	for(int index = 0; index < 4; ++index) {
		int dir = RkDir[index];
		int t_sq = sq + dir;
		Piece pce = m_board[t_sq];
		while(!isOffboard(t_sq)) {
			if(pce != EMPTY) {
				if(Board::PieceRookQueen[pce] && PieceCol[pce] == side) {
					return true;
				}
				break;
			}
			t_sq += dir;
			pce = m_board[t_sq];
		}
	}

	// bishops, queens
	for(int index = 0; index < 4; ++index) {
		int dir = BiDir[index];
		int t_sq = sq + dir;
		int pce = m_board[t_sq];
		while(!isOffboard(t_sq)) {
			if(pce != EMPTY) {
				if(Board::PieceBishopQueen[pce] && PieceCol[pce] == side) {
					return true;
				}
				break;
			}
			t_sq += dir;
			pce = m_board[t_sq];
		}
	}

	// kings
	for(int index = 0; index < 8; ++index) {
		Piece pce = m_board[sq + KiDir[index]];
		if(Board::PieceKing[pce] && PieceCol[pce]==side) {
			return true;
		}
	}

	return false;
}
