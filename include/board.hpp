#ifndef BOARD_H
#define BOARD_H

#include <stdint.h> //Standard Integer Types
#include "types.hpp"

/*
   MACROS
*/ 
#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) )          // Convert file and rank to square

#define SQ64(sq120) (Bitboard::Sq120ToSq64[(sq120)])       // Convert 120 base board index to 64
#define SQ120(sq64) (Bitboard::Sq64ToSq120[(sq64)])        // Convery 64 base board index to 120
#define CLRBIT(bb,sq) ((bb) &= Bitboard::ClearMask[(sq)])  // Clear bitboard bit
#define SETBIT(bb,sq) ((bb) |= Bitboard::SetMask[(sq)])    // Set bitboard bit


typedef uint64_t bitboard; // bitboard type

namespace Bitboard{

static uint64_t SetMask[64];
static uint64_t ClearMask[64];

static uint8_t Sq120ToSq64[TOTAL_SQUARES];  // Map of base 120 indexes to base 64
static uint8_t Sq64ToSq120[64];             // Map of base 64 indexes to base 120


// Dark square bitboard mask
const bitboard DarkSquares = 0xAA55AA55AA55AA55;

// File bitboard mask
const bitboard FileA_Mask = 0x0101010101010101ULL;
const bitboard FileB_Mask = FileA_Mask << 1;
const bitboard FileC_Mask = FileA_Mask << 2;
const bitboard FileD_Mask = FileA_Mask << 3;
const bitboard FileE_Mask = FileA_Mask << 4;
const bitboard FileF_Mask = FileA_Mask << 5;
const bitboard FileG_Mask = FileA_Mask << 6;
const bitboard FileH_Mask = FileA_Mask << 7;

// Rank bitboard mask
const bitboard Rank1_Mask = 0xFF;
const bitboard Rank2_Mask = Rank1_Mask << (8 * 1);
const bitboard Rank3_Mask = Rank1_Mask << (8 * 2);
const bitboard Rank4_Mask = Rank1_Mask << (8 * 3);
const bitboard Rank5_Mask = Rank1_Mask << (8 * 4);
const bitboard Rank6_Mask = Rank1_Mask << (8 * 5);
const bitboard Rank7_Mask = Rank1_Mask << (8 * 6);
const bitboard Rank8_Mask = Rank1_Mask << (8 * 7);

uint8_t popBit(bitboard *bb);    // returns the index of least signifcant bit and sets it to zero

uint8_t countBits(bitboard b);   // Counts the number of bits which are set in the bitboard

void printBitboard(bitboard bb); // Prints bitboard to stdout

} // namespace

class Board {
private:
// Disable copy and assign
Board& operator=(const Board&);
Board(const Board&);

void seedRandNums();        // Seed rand() and set keys for hashing
void initBitMasks() const;  // Generate Masks for Bitboards
void init120To64() const;   // Generate 120 to 64 bit mapping
void clearBoard();          // Clears all member data
inline bool isOffboard(const uint8_t sq120) const; // is square offboard

public:


/*
  Piece Directions
*/
static const int KnDir[8];
static const int RkDir[4];
static const int BiDir[4];
static const int KiDir[8];

/*                                            
   Arrays of form {  EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
                       0     1   2   3   4   5   6   7   8   9  10  11  12
*/
static const bool PieceMaj[13];         // King, Queen, Rook,
static const bool PieceMin[13];         // Knight, Bishop
static const unsigned int PieceVal[13]; // Piece values
static const Colour PieceCol[13];       // Piece Colours

static const bool PieceKnight[13];      // Knights
static const bool PieceKing[13];        // Kings
static const bool PieceRookQueen[13];   // Rooks and Queens
static const bool PieceBishopQueen[13]; // Bishops and Queens

static const bool PieceSlides[13]; // piece is a sliding piece, i.e. queen, rook, bishop
static uint8_t FilesBrd[TOTAL_SQUARES]; // file index of each square
static uint8_t RanksBrd[TOTAL_SQUARES]; // rank index of each square

/*
  Strings for pretty printing
*/
static const char* PceChar;
static const char* SideChar;
static const char* RankChar;
static const char* FileChar;

Board();

static void initStaticMembers();   // sets m_FilesBrd and m_RanksBrd


// Prints board to stdout
void printBoard() const;

// Generate Zobrist hash key from board poistion
uint64_t genHashKey() const; 

// Sets up board based on FEN string
bool parseFen(char *fen);

// Returns true if given square is attacked by colour paramter
bool sqAttacked(const uint8_t sq, Colour size);

// Returns true if board has a valid position
bool checkBoard() const;

// update material member data
void updateListsMaterial();

// Returns true if Square 'sq' is attacked by a piece of Colour side
bool isSquareAttacked(const Square& sq, const Colour& side) const;

Colour m_side;                 // Side to play colour
Square m_enPas;                // Square available for en Passent
Piece m_board[TOTAL_SQUARES];  // Piece on each square(120 squares)
uchar m_castling;              // Potential castling moves, defined using flags 
Square m_kingSq[2];            // White and Black king squares
uint32_t m_fiftyMove;          // Fifty moves without capture or promotion draw counter

uint32_t m_ply;                // number of half moves
uint32_t m_hisply; 

uint64_t m_posHash;            // Zorbist has of the position

uint32_t m_majPce[2];          // Number of major pieces
uint32_t m_minPce[2];          // Number of minor pieces

// CAN THIS BE REMOVED USING COUNT BITBOARD BITS INSTEAD
uint32_t m_pceNum[13];         // Number of each peice type on board 
//
///////////////////////////////////////////////////////////////////

uint32_t m_material[2];        // total peiece values of players pieces

bitboard m_pList[13];          // Bitboards for each piece type

// Rand hashing vals
uint64_t pieceKeys[13][TOTAL_SQUARES];
uint64_t sideKey;
uint64_t castleKeys[16];

}; //Board

#endif // BOARD_H
