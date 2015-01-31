#include "board.hpp"
#include <cassert>
#include <ctime>
#include <cstdlib>

#include <iostream>
#include <iomanip>

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

const char* Board::PceChar = ".PNBRQKpnbrqk";
const char* Board::SideChar = "wb-";
const char* Board::RankChar = "12345678";
const char* Board::FileChar = "abcdefgh";


Board::Board()
{
    seedRandNums();
    initBitMasks(); // Generate Masks for Bitboards
    init120To64(); // Generate 120 to 64 bit mapping

    // Setup starting board position
    clearBoard();
}

void Board::clearBoard()
{
	
	for(uint8_t index = 0; index < TOTAL_SQUARES; ++index) {
		m_board[index] = EMPTY;
	}
	
	for(uint8_t index = 0; index < 2; ++index) {
		m_bigPce[index] = 0;
		m_majPce[index] = 0;
		m_minPce[index] = 0;
	    m_material[index] = 0;
		m_pawns[index] = 0ULL;
	}
	
	for(uint8_t index = 0; index < 13; ++index) {
		m_pceNum[index] = 0;
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

void Board::printBoard() const 
{
	
	
	std::cout <<"\nGame Board:\n\n";
	
	for(int rank = RANK_8; rank >= RANK_1; rank--) {
		std::cout<< rank+1 << " ";
		for(int file = FILE_A; file <= FILE_H; file++) {
			uint8_t sq = FR2SQ(file,rank);
			uint8_t piece = m_board[sq];
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

