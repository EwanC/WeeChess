#ifndef BOARD_H
#define BOARD_H

#include <stdint.h> //Standard Integer Types
#include <string>
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

const uint64_t SetMask[64] = {
  0x0000000000000001,
  0x0000000000000002,
  0x0000000000000004,
  0x0000000000000008,
  0x0000000000000010,
  0x0000000000000020,
  0x0000000000000040,
  0x0000000000000080,
  0x0000000000000100,
  0x0000000000000200,
  0x0000000000000400,
  0x0000000000000800,
  0x0000000000001000,
  0x0000000000002000,
  0x0000000000004000,
  0x0000000000008000,
  0x0000000000010000,
  0x0000000000020000,
  0x0000000000040000,
  0x0000000000080000,
  0x0000000000100000,
  0x0000000000200000,
  0x0000000000400000,
  0x0000000000800000,
  0x0000000001000000,
  0x0000000002000000,
  0x0000000004000000,
  0x0000000008000000,
  0x0000000010000000,
  0x0000000020000000,
  0x0000000040000000,
  0x0000000080000000,
  0x0000000100000000,
  0x0000000200000000,
  0x0000000400000000,
  0x0000000800000000,
  0x0000001000000000,
  0x0000002000000000,
  0x0000004000000000,
  0x0000008000000000,
  0x0000010000000000,
  0x0000020000000000,
  0x0000040000000000,
  0x0000080000000000,
  0x0000100000000000,
  0x0000200000000000,
  0x0000400000000000,
  0x0000800000000000,
  0x0001000000000000,
  0x0002000000000000,
  0x0004000000000000,
  0x0008000000000000,
  0x0010000000000000,
  0x0020000000000000,
  0x0040000000000000,
  0x0080000000000000,
  0x0100000000000000,
  0x0200000000000000,
  0x0400000000000000,
  0x0800000000000000,
  0x1000000000000000,
  0x2000000000000000,
  0x4000000000000000,
  0x8000000000000000
};

// ClearMask[i] = ~SetMask[i]
const uint64_t ClearMask[64]= {
  0xfffffffffffffffe,
  0xfffffffffffffffd,
  0xfffffffffffffffb,
  0xfffffffffffffff7,
  0xffffffffffffffef,
  0xffffffffffffffdf,
  0xffffffffffffffbf,
  0xffffffffffffff7f,
  0xfffffffffffffeff,
  0xfffffffffffffdff,
  0xfffffffffffffbff,
  0xfffffffffffff7ff,
  0xffffffffffffefff,
  0xffffffffffffdfff,
  0xffffffffffffbfff,
  0xffffffffffff7fff,
  0xfffffffffffeffff,
  0xfffffffffffdffff,
  0xfffffffffffbffff,
  0xfffffffffff7ffff,
  0xffffffffffefffff,
  0xffffffffffdfffff,
  0xffffffffffbfffff,
  0xffffffffff7fffff,
  0xfffffffffeffffff,
  0xfffffffffdffffff,
  0xfffffffffbffffff,
  0xfffffffff7ffffff,
  0xffffffffefffffff,
  0xffffffffdfffffff,
  0xffffffffbfffffff,
  0xffffffff7fffffff,
  0xfffffffeffffffff,
  0xfffffffdffffffff,
  0xfffffffbffffffff,
  0xfffffff7ffffffff,
  0xffffffefffffffff,
  0xffffffdfffffffff,
  0xffffffbfffffffff,
  0xffffff7fffffffff,
  0xfffffeffffffffff,
  0xfffffdffffffffff,
  0xfffffbffffffffff,
  0xfffff7ffffffffff,
  0xffffefffffffffff,
  0xffffdfffffffffff,
  0xffffbfffffffffff,
  0xffff7fffffffffff,
  0xfffeffffffffffff,
  0xfffdffffffffffff,
  0xfffbffffffffffff,
  0xfff7ffffffffffff,
  0xffefffffffffffff,
  0xffdfffffffffffff,
  0xffbfffffffffffff,
  0xff7fffffffffffff,
  0xfeffffffffffffff,
  0xfdffffffffffffff,
  0xfbffffffffffffff,
  0xf7ffffffffffffff,
  0xefffffffffffffff,
  0xdfffffffffffffff,
  0xbfffffffffffffff,
  0x7fffffffffffffff
};
// Map of base 64 indexes to base 120
const int Sq64ToSq120[64] = {
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  58,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  71,
  72,
  73,
  74,
  75,
  76,
  77,
  78,
  81,
  82,
  83,
  84,
  85,
  86,
  87,
  88,
  91,
  92,
  93,
  94,
  95,
  96,
  97,
  98
};


// Map of base 120 indexes to base 64       
const int Sq120ToSq64[TOTAL_SQUARES] = {
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  65,
  65,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  65,
  65,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  65,
  65,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  65,
  65,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  65,
  65,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  65,
  65,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  65,
  65,
  56,
  57,
  58,
  59,
  60,
  61,
  62,
  63,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65,
  65
};

  
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

int popBit(bitboard *bb);    // returns the index of least signifcant bit and sets it to zero

int countBits(bitboard b);   // Counts the number of bits which are set in the bitboard

void printBitboard(bitboard bb); // Prints bitboard to stdout

} // namespace


struct Undo{
  int move;
  uchar castlingPerm;
  Square enPas;
  uint32_t fiftyMove;
  uint64_t posHash;
};

class Board {
private:
// Disable copy and assign
Board& operator=(const Board&);
Board(const Board&);

void seedRandNums();        // Seed rand() and set keys for hashing
void clearBoard();          // Clears all member data
inline bool isOffboard(const int sq120) const; // is square offboard

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
static int FilesBrd[TOTAL_SQUARES]; // file index of each square
static int RanksBrd[TOTAL_SQUARES]; // rank index of each square

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
bool parseFen(std::string fen);

// Returns true if given square is attacked by colour paramter
bool sqAttacked(const int sq, Colour size);

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

uint32_t m_ply;                // number of half moves  in current search
uint32_t m_hisply;             // number of half moves in entire games

uint64_t m_posHash;            // Zorbist has of the position

uint32_t m_majPce[2];          // Number of major pieces
uint32_t m_minPce[2];          // Number of minor pieces

uint32_t m_material[2];        // total peiece values of players pieces

bitboard m_pList[13];          // Bitboards for each piece type

// Rand hashing vals
uint64_t pieceKeys[13][TOTAL_SQUARES];
uint64_t sideKey;
uint64_t castleKeys[16];

Undo m_history[MAX_GAME_MOVES];

PVTable m_pvTable;

int m_searchHistory[13][TOTAL_SQUARES];
int m_searchKillers[2][MAXDEPTH];

}; //Board

#endif // BOARD_H
