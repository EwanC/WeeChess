#ifndef BOARD_H
#define BOARD_H

#include <stdint.h> //Standard Integer Types
#include <string>
#include "types.hpp"
#include "pvtable.hpp"
#include "boardStaticData.hpp"

/*
   MACROS
*/
#define FR2SQ(f, r) ((21 + (f)) + ((r)*10)) // Convert file and rank to square

#define SQ64(sq120) (Bitboard::Sq120ToSq64[(sq120)])       // Convert 120 base board index to 64
#define SQ120(sq64) (Bitboard::Sq64ToSq120[(sq64)])        // Convery 64 base board index to 120
#define CLRBIT(bb, sq) ((bb) &= Bitboard::ClearMask[(sq)]) // Clear bitboard bit
#define SETBIT(bb, sq) ((bb) |= Bitboard::SetMask[(sq)])   // Set bitboard bit

namespace Bitboard {

    int popBit(bitboard* bb); // returns the index of least signifcant bit and sets it to zero

    int countBits(bitboard b); // Counts the number of bits which are set in the bitboard

    void printBitboard(bitboard bb); // Prints bitboard to stdout

} // namespace

// Inividual records in move history
struct Undo {
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

    void seedRandNums();                           // Seed rand() and set keys for hashing
    void clearBoard();                             // Clears all member data
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

    static const bool PieceSlides[13];  // piece is a sliding piece, i.e. queen, rook, bishop
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

    static void initStaticMembers(); // sets m_FilesBrd and m_RanksBrd

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

    void MirrorBoard();
    void resetBoard();

    Colour m_side;                // Side to play colour
    Square m_enPas;               // Square available for en Passent
    Piece m_board[TOTAL_SQUARES]; // Piece on each square(120 squares)
    uchar m_castling;             // Potential castling moves, defined using flags
    Square m_kingSq[2];           // White and Black king squares
    uint32_t m_fiftyMove;         // Fifty moves without capture or promotion draw counter

    uint32_t m_ply; // number of half moves in CURRENT SEARCH
    int m_hisply;   // number of half moves in entire games

    uint64_t m_posHash; // Zorbist hash of the position

    uint32_t m_majPce[2]; // Number of major pieces
    uint32_t m_minPce[2]; // Number of minor pieces

    uint32_t m_material[2]; // total peiece values of players pieces

    bitboard m_pList[13]; // Bitboards for each piece type

    // Rand hashing vals
    uint64_t pieceKeys[13][TOTAL_SQUARES];
    uint64_t sideKey;
    uint64_t castleKeys[16];

    Undo m_history[MAX_GAME_MOVES]; // History of moves made

    PVTable m_pvTable; // Principle variation table of best moves found.

    // AlphaBeta heuristics used for move order
    int m_searchHistory[13][TOTAL_SQUARES]; // Indexed by piece type and square
                                            // everytime a move improves on alpha in current search
                                            // incremented by one

    int m_searchKillers[2][MAXDEPTH]; // Stores to move that most recently caused beta cutoff, but
                                      // aren't captures

}; // Board

#endif // BOARD_H
