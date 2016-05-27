#include "eval.hpp"
#include "search.hpp"

#include "ocl.hpp"
#include <cassert>

// Uses Piece table to evaulate position
int Eval::evalPosition(const Board& b)
{
    // material score
    int score = b.m_material[WHITE] - b.m_material[BLACK];
    if (!Bitboard::countBits(b.m_pList[wP]) && !Bitboard::countBits(b.m_pList[bP]) && Eval::materialDraw(b) == true)
    {
        return 0;
    }

    score += OCL::getInstance()->RunEvalKernel(b);

    if (b.m_side == WHITE)
    {
        return score;
    }
    else
    {
        return -score;
    }
}

//  Returns true if based on material left game is a draw
bool Eval::materialDraw(const Board& b)
{
    assert(b.checkBoard());

    const int numbR = Bitboard::countBits(b.m_pList[bR]);
    const int numwR = Bitboard::countBits(b.m_pList[wR]);
    const int numbQ = Bitboard::countBits(b.m_pList[bQ]);
    const int numwQ = Bitboard::countBits(b.m_pList[wQ]);
    const int numwN = Bitboard::countBits(b.m_pList[wN]);
    const int numbN = Bitboard::countBits(b.m_pList[bN]);
    const int numwB = Bitboard::countBits(b.m_pList[wB]);
    const int numbB = Bitboard::countBits(b.m_pList[bB]);

    if (numwR != 0 && numbR != 0 && numwQ != 0 && numbQ != 0)
    {
        if (numbB != 0 && numwB != 0)
        {
            if (numwN < 3 && numbN < 3)
            {
                return true;
            }
        }
        else if (numwN != 0 && numbN != 0)
        {
            if (abs(numwB - numbB) < 2)
            {
                return true;
            }
        }
        else if ((numwN < 3 && numwB != 0) || (numwB == 1 && numwN != 0))
        {
            if ((numbN < 3 && numbB != 0) || (numbB == 1 && numbN != 0))
            {
                return true;
            }
        }
    }
    else if (numwQ != 0 && numbQ != 0)
    {
        if (numwR == 1 && numbR == 1)
        {
            if (numwN + numwB < 2 && numbN + numbB < 2)
            {
                return true;
            }
        }
        else if (numwR == 1 && numbR != 0)
        {
            if ((numwN + numwB == 0 && numbB == 1) || (numbN + numbB == 2))
            {
                return true;
            }
        }
        else if (numbR == 1 && numwR != 0)
        {
            if (numbN + numbB == 0 && ((numwN + numbN == 1) || (numwN + numwB == 2)))
            {
                return true;
            }
        }
    }
    return false;
}
