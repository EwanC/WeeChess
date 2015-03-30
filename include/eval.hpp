#ifndef EVAL_H
#define EVAL_H

#include "board.hpp"

#define ENDGAME_MAT                                                                                \
    (1 * Board::PieceVal[wR] + 2 * Board::PieceVal[wN] + 2 * Board::PieceVal[wP] +                 \
     Board::PieceVal[wK])

namespace Eval {

    int evalPosition(const Board& b);  // Uses Piece table to evaulate position
    bool materialDraw(const Board& b); //  Returns true if based on material left game is a draw
    void initEvalMasks();              // Init evaluation masks

    // How to prioritize captures - MvvLva
    // Most Valuable Victim, Least Valuavle attacker
    const int VictimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};

    /*
        Piece tables used for basic evaluation of board positions
    */

    const int PawnTable[64] = {0,  0,  0,  0,  0,  0,  0,  0,  10, 10, 0,  -10, -10, 0,  10, 10,
                               5,  0,  0,  5,  5,  0,  0,  5,  0,  0,  10, 20,  20,  10, 0,  0,
                               5,  5,  5,  10, 10, 5,  5,  5,  10, 10, 10, 20,  20,  10, 10, 10,
                               20, 20, 20, 30, 30, 20, 20, 20, 0,  0,  0,  0,   0,   0,  0,  0};

    const int KnightTable[64] = {0, -10, 0,  0,  0,  0,  -10, 0, 0, 0,  0,  5,  5,  0,  0,  0,
                                 0, 0,   10, 10, 10, 10, 0,   0, 0, 0,  10, 20, 20, 10, 5,  0,
                                 5, 10,  15, 20, 20, 15, 10,  5, 5, 10, 10, 20, 20, 10, 10, 5,
                                 0, 0,   5,  10, 10, 5,  0,   0, 0, 0,  0,  0,  0,  0,  0,  0};

    const int BishopTable[64] = {0, 0,  -10, 0,  0,  -10, 0,  0, 0, 0,  0,  10, 10, 0,  0,  0,
                                 0, 0,  10,  15, 15, 10,  0,  0, 0, 10, 15, 20, 20, 15, 10, 0,
                                 0, 10, 15,  20, 20, 15,  10, 0, 0, 0,  10, 15, 15, 10, 0,  0,
                                 0, 0,  0,   10, 10, 0,   0,  0, 0, 0,  0,  0,  0,  0,  0,  0};

    const int RookTable[64] = {0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                               0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                               0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                               25, 25, 25, 25, 25, 25, 25, 25, 0, 0, 5, 10, 10, 5, 0, 0};

    // Mirror table for conveting black piece locations into white, so
    // black cacn used the same evaluation piece tables
    const int Mirror64[64] = {56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50, 51, 52, 53, 54, 55,
                              40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39,
                              24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23,
                              8,  9,  10, 11, 12, 13, 14, 15, 0,  1,  2,  3,  4,  5,  6,  7};

    const int KingE[64] = {-50, -10, 0,  0,  0,  0,  -10, -50, -10, 0,   10, 10, 10, 10, 0,   -10,
                           0,   10,  20, 20, 20, 20, 10,  0,   0,   10,  20, 40, 40, 20, 10,  0,
                           0,   10,  20, 40, 40, 20, 10,  0,   0,   10,  20, 20, 20, 20, 10,  0,
                           -10, 0,   10, 10, 10, 10, 0,   -10, -50, -10, 0,  0,  0,  0,  -10, -50};

    const int KingO[64] = {0,   5,   5,   -10, -10, 0,   10,  5,   -30, -30, -30, -30, -30,
                           -30, -30, -30, -50, -50, -50, -50, -50, -50, -50, -50, -70, -70,
                           -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70,
                           -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70,
                           -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70};

} // End namespace Eval

#endif // EVAL_H