#include "board.hpp"
#include <cassert>
#include <ctime>
#include <cstdlib>

#define RAND_64 ((uint64_t)rand() | \
			     (uint64_t)rand() << 15 | \
				 (uint64_t)rand() << 30 | \
				 (uint64_t)rand() << 45 | \
				 ((uint64_t)rand() & 0xf) << 60 )  

namespace Bitboard{

uint64_t SetMask[64];
uint64_t ClearMask[64];

uint8_t Sq120ToSq64[TOTAL_SQUARES];
uint8_t Sq64ToSq120[64];

}


Board::Board()
{
    seedRandNums();
    initBitMasks(); // Generate Masks for Bitboards
    init120To64(); // Generate 120 to 64 bit mapping

    // Setup start board 
}

void Board::seedRandNums()
{
    srand(time(NULL));
   
	for(uint8_t index = 0; index < 13; ++index) {
		for(uint8_t index2 = 0; index2 < 120; ++index2) {
			pieceKeys[index][index2] = RAND_64;
		}
	}
	sideKey = RAND_64;
	for(uint8_t index = 0; index < 16; ++index) {
		castleKeys[index] = RAND_64;
	}
 

}


// TODO - Set these at compile time, can't use constexpr currently
void Board::initBitMasks() const
{
	
	for(uint8_t index = 0; index < 64; index++) {
		Bitboard::SetMask[index] = 0ULL;
		Bitboard::ClearMask[index] = 0ULL;
	}
	
	for(uint8_t index = 0; index < 64; index++) {
		Bitboard::SetMask[index] |= (1ULL << index);
		Bitboard::ClearMask[index] = ~Bitboard::SetMask[index];
	}

}

//TODO - Set these at compile time, can't use constexpr currently
void Board::init120To64() const
{   
	int sq = A1;
	int sq64 = 0;
	for(uint8_t index = 0; index < TOTAL_SQUARES; ++index) {
		Bitboard::Sq120ToSq64[index] = 65;
	}
	
	for(uint8_t index = 0; index < 64; ++index) {
		Bitboard::Sq64ToSq120[index] = 120;
	}
	
	for(uint8_t rank = RANK_1; rank <= RANK_8; ++rank) {
		for(uint8_t file = FILE_A; file <= FILE_H; ++file) {
			uint8_t sq = FR2SQ(file,rank);
			Bitboard::Sq64ToSq120[sq64] = sq;
			Bitboard::Sq120ToSq64[sq] = sq64;
			sq64++;
		}
	}

}


uint64_t Board::genHashKey()
{
	uint64_t finalKey = 0;
	
	for(uint8_t sq = 0; sq < TOTAL_SQUARES; ++sq) {
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

