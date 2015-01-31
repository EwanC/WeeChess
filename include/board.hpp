#ifndef BOARD_H
#define BOARD_H

#include <stdint.h> //Standard Integer Types
#include "types.hpp"

// MACROS 
#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) ) 
#define SQ64(sq120) (Bitboard::Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Bitboard::Sq64ToSq120[(sq64)])
#define CLRBIT(bb,sq) ((bb) &= Bitboard::ClearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= Bitboard::SetMask[(sq)])

typedef uint64_t bitboard;

namespace Bitboard{

const bitboard DarkSquares = 0xAA55AA55AA55AA55;

const bitboard FileA_Mask = 0x0101010101010101ULL;
const bitboard FileB_Mask = FileA_Mask << 1;
const bitboard FileC_Mask = FileA_Mask << 2;
const bitboard FileD_Mask = FileA_Mask << 3;
const bitboard FileE_Mask = FileA_Mask << 4;
const bitboard FileF_Mask = FileA_Mask << 5;
const bitboard FileG_Mask = FileA_Mask << 6;
const bitboard FileH_Mask = FileA_Mask << 7;

const bitboard Rank1_Mask = 0xFF;
const bitboard Rank2_Mask = Rank1_Mask << (8 * 1);
const bitboard Rank3_Mask = Rank1_Mask << (8 * 2);
const bitboard Rank4_Mask = Rank1_Mask << (8 * 3);
const bitboard Rank5_Mask = Rank1_Mask << (8 * 4);
const bitboard Rank6_Mask = Rank1_Mask << (8 * 5);
const bitboard Rank7_Mask = Rank1_Mask << (8 * 6);
const bitboard Rank8_Mask = Rank1_Mask << (8 * 7);

}

class Board {
private:
// Disable copy and assign
Board& operator=(const Board&);
Board(const Board&);

// Rand hashing vals
uint64_t pieceKeys[13][TOTAL_SQUARES];
uint64_t sideKey;
uint64_t castleKeys[16];

void seedRandNums();
void initBitMasks() const; // Generate Masks for Bitboards
void init120To64() const; // Generate 120 to 64 bit mapping
void clearBoard();

static const char* PceChar;
static const char* SideChar;
static const char* RankChar;
static const char* FileChar;

public:

Board();
void printBoard() const;

// Generate Zobrist hash key from board poistion
uint64_t genHashKey();



Colour m_side;
Square m_enPas;
Piece m_board[TOTAL_SQUARES];
uchar m_castling;
Square m_kingSq[2];
uint32_t m_fiftyMove;

uint32_t m_ply;
uint32_t m_hisply;

uint64_t m_posHash;

uint64_t m_pawns[2];
uint32_t m_bigPce[2];
uint32_t m_majPce[2];
uint32_t m_minPce[2];

uint32_t m_pceNum[13];
uint32_t m_material[2];


}; //Board

#endif // BOARD_H
