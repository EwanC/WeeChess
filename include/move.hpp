#ifndef MOVE_H
#define MOVE_H

#include <stdint.h> //Standard Integer Types
#include <string>
#include <vector>
#include "types.hpp"

class Board;

/* GAME MOVE

   0000 0000 0000 0000 0000 0111 1111 -> From [0x7F]
   0000 0000 0000 0011 1111 1000 0000 -> To [>> 7, 0x7F]
   0000 0000 0011 1100 0000 0000 0000 -> Captured Piece [>> 14, 0xF]
   0000 0000 0100 0000 0000 0000 0000 -> En Passent capture? [0x40000]
   0000 0000 1000 0000 0000 0000 0000 -> Pawn Start? [0x80000]
   0000 1111 0000 0000 0000 0000 0000 -> Piece promoted to[>> 20, 0xF]
   0001 0000 0000 0000 0000 0000 0000 -> Castle? [0x1000000]

*/

#define FROMSQ(m) ((m)&0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MFLAGEP 0x40000   // Move flag En Pass
#define MFLAGPS 0x80000   // Move flag pawn start
#define MFLAGCA 0x1000000 // Move flag castle

#define MFLAGCAP 0x7C000   // Move flag captured
#define MFLAGPROM 0xF00000 // Move flag promoted

// Generates move int for paramters
#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (Board::FilesBrd[(sq)] == OFFBOARD)

// Hashing macros
#define HASH_PCE(pce, sq) (b.m_posHash ^= (b.pieceKeys[(pce)][(sq)])) // hash piece
#define HASH_CA (b.m_posHash ^= (b.castleKeys[(b.m_castling)]))       // hash castling
#define HASH_SIDE (b.m_posHash ^= (b.sideKey))                        // hash side
#define HASH_EP (b.m_posHash ^= (b.pieceKeys[EMPTY][(b.m_enPas)]))    // hash enpassent

class Move {

  public:
    Move();

    uint32_t m_move;
    int m_score;

    std::string moveString() const; // Converts move into string for printing
};

class MoveList {

    int MvvLvaScores[13][13]; // Most Valuable victim, least valuable attacker

  public:
    MoveList(); // Consultors, inits search MvvLva scores

    void genAllMoves(const Board& b); // Generate all moves for a board position
    void genAllCaps(const Board& b);  //  Generate all capture moves
    void addQuietMove(const Board& b, uint32_t move);
    void addCaptureMove(const Board& b, uint32_t move);
    void addEnPassantMove(const Board& b, uint32_t move);

    void printList() const;

    template <Colour colour> void addPawnMove(const Board& b, const int from, const int to);

    template <Colour colour>
    void addPawnCapMove(const Board& b, const int from, const int to, const Piece cap);

    std::vector<Move> m_move_vec;
};

namespace MoveGen {
    void takeMove(Board& b);           // Takes back last move
    bool makeMove(Board& b, int move); // Modifies board according to move
    void movePiece(const int from, const int to,
                   Board& b); // Moves piece from a 120sq to another 120sq
    void addPiece(const int sq, Board& b, const Piece pce); // adds piece to the board at 120sq
    void clearPiece(const int sq, Board& b);                // clears piece from board at 120 sq
    bool moveExists(Board& b, const int move);              // Checks if move is valid
    uint32_t parseMove(char* ptrChar, Board& b);            // parses algebraic move
    void takeNullMove(Board& b);                            // Undo null move
    void makeNullMove(Board& b);                            // Pass on move if not in check
    std::string moveString(uint32_t move); // Converts move into string for printing
}

#endif // MOVE_H