#include "eval.hpp"
#include "search.hpp"

#include <cassert>

namespace Eval {

    uint64_t FileBBMask[8];
    uint64_t RankBBMask[8];

    uint64_t BlackPassedMask[64];
    uint64_t WhitePassedMask[64];
    uint64_t IsolatedMask[64];

    const int PawnIsolated = -10;
    const int PawnPassed[8] = {0, 5, 10, 20, 35, 60, 100, 200};
    const int RookOpenFile = 10;
    const int RookSemiOpenFile = 5;
    const int QueenOpenFile = 5;
    const int QueenSemiOpenFile = 3;
    const int BishopPair = 30;
}

// Uses Piece table to evaulate position
int Eval::evalPosition(const Board& b)
{

    Piece pce;

    // material score
    int score = b.m_material[WHITE] - b.m_material[BLACK];
    if (!Bitboard::countBits(b.m_pList[wP]) && !Bitboard::countBits(b.m_pList[bP]) &&
        Eval::materialDraw(b) == true) {
        return 0;
    }

    // White pawns
    bitboard wpBitboard = b.m_pList[wP];
    int numPce = Bitboard::countBits(wpBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&wpBitboard);
        score += PawnTable[sq64];

        if ((Eval::IsolatedMask[sq64] & wpBitboard) == 0) {
            score += Eval::PawnIsolated;
        }

        if ((Eval::WhitePassedMask[sq64] & b.m_pList[bP]) == 0) {
            score += Eval::PawnPassed[Board::RanksBrd[sq64]];
        }
    }

    // Black pawns
    bitboard bpBitboard = b.m_pList[bP];
    numPce = Bitboard::countBits(bpBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&bpBitboard);
        score -= PawnTable[Mirror64[sq64]];

        if ((Eval::IsolatedMask[sq64] & bpBitboard) == 0) {
            score -= Eval::PawnIsolated;
        }

        if ((Eval::BlackPassedMask[sq64] & b.m_pList[wP]) == 0) {
            score -= Eval::PawnPassed[7 - Board::RanksBrd[sq64]];
        }
    }

    // White Knight
    bitboard wnBitboard = b.m_pList[wN];
    numPce = Bitboard::countBits(wnBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&wnBitboard);
        score += KnightTable[sq64];
    }

    // Black Knight
    bitboard bnBitboard = b.m_pList[bN];
    numPce = Bitboard::countBits(bnBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&bnBitboard);
        score -= KnightTable[Mirror64[sq64]];
    }

    // White Bishop
    bitboard wbBitboard = b.m_pList[wB];
    numPce = Bitboard::countBits(wbBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&wbBitboard);
        score += BishopTable[sq64];
    }

    // Black Bishop
    bitboard bbBitboard = b.m_pList[bB];
    numPce = Bitboard::countBits(bbBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&bbBitboard);
        score -= BishopTable[Mirror64[sq64]];
    }

    // White Rook
    bitboard wrBitboard = b.m_pList[wR];
    numPce = Bitboard::countBits(wrBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&wrBitboard);
        int sq120 = SQ120(sq64);
        score += RookTable[sq64];

        assert(Board::FilesBrd[sq120] >= 0 && Board::FilesBrd[sq120] <= 7);

        if (!((b.m_pList[bP] | b.m_pList[wP]) & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score += Eval::RookOpenFile;
        }
        else if (!(b.m_pList[wP] & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score += Eval::RookSemiOpenFile;
        }
    }

    // Black Rook
    bitboard brBitboard = b.m_pList[bR];
    numPce = Bitboard::countBits(brBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&brBitboard);
        int sq120 = SQ120(sq64);
        score -= RookTable[Mirror64[sq64]];

        assert(Board::FilesBrd[sq120] >= 0 && Board::FilesBrd[sq120] <= 7);
        if (!((b.m_pList[bP] | b.m_pList[wP]) & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score -= Eval::RookOpenFile;
        }
        else if (!(b.m_pList[bP] & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score -= Eval::RookSemiOpenFile;
        }
    }

    // White Queen
    bitboard wqBitboard = b.m_pList[bR];
    numPce = Bitboard::countBits(wqBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&wqBitboard);
        int sq120 = SQ120(sq64);
        assert(Board::FilesBrd[sq120] >= 0 && Board::FilesBrd[sq120] <= 7);

        if (!((b.m_pList[bP] | b.m_pList[wP]) & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score += Eval::QueenOpenFile;
        }
        else if (!(b.m_pList[wP] & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score += Eval::QueenSemiOpenFile;
        }
    }

    // Black Queen
    bitboard bqBitboard = b.m_pList[bQ];
    numPce = Bitboard::countBits(bqBitboard);
    for (int pceNum = 0; pceNum < numPce; ++pceNum) {
        int sq64 = Bitboard::popBit(&bqBitboard);
        int sq120 = SQ120(sq64);
        assert(Board::FilesBrd[sq120] >= 0 && Board::FilesBrd[sq120] <= 7);

        if (!((b.m_pList[bP] | b.m_pList[wP]) & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score -= Eval::QueenOpenFile;
        }
        else if (!(b.m_pList[bP] & Eval::FileBBMask[Board::FilesBrd[sq120]])) {
            score -= Eval::QueenSemiOpenFile;
        }
    }

    // White King
    bitboard wkBitboard = b.m_pList[wK];
    int sq64 = Bitboard::popBit(&wkBitboard);
    if ((b.m_material[BLACK] <= ENDGAME_MAT)) {
        score += Eval::KingE[sq64];
    }
    else {
        score += Eval::KingO[sq64];
    }

    // Black King
    bitboard bkBitboard = b.m_pList[bK];
    sq64 = Bitboard::popBit(&bkBitboard);

    if ((b.m_material[WHITE] <= ENDGAME_MAT)) {
        score -= Eval::KingE[Mirror64[sq64]];
    }
    else {
        score -= Eval::KingO[Mirror64[sq64]];
    }

    if (Bitboard::countBits(wbBitboard) >= 2)
        score += Eval::BishopPair;

    if (Bitboard::countBits(bbBitboard) >= 2)
        score -= Eval::BishopPair;

    if (b.m_side == WHITE) {
        return score;
    }
    else {
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

    if (numwR != 0 && numbR != 0 && numwQ != 0 && numbQ != 0) {
        if (numbB != 0 && numwB != 0) {
            if (numwN < 3 && numbN < 3) {
                return true;
            }
        }
        else if (numwN != 0 && numbN != 0) {
            if (abs(numwB - numbB) < 2) {
                return true;
            }
        }
        else if ((numwN < 3 && numwB != 0) || (numwB == 1 && numwN != 0)) {
            if ((numbN < 3 && numbB != 0) || (numbB == 1 && numbN != 0)) {
                return true;
            }
        }
    }
    else if (numwQ != 0 && numbQ != 0) {
        if (numwR == 1 && numbR == 1) {
            if (numwN + numwB < 2 && numbN + numbB < 2) {
                return true;
            }
        }
        else if (numwR == 1 && numbR != 0) {
            if ((numwN + numwB == 0 && numbB == 1) || (numbN + numbB == 2)) {
                return true;
            }
        }
        else if (numbR == 1 && numwR != 0) {
            if (numbN + numbB == 0 && ((numwN + numbN == 1) || (numwN + numwB == 2))) {
                return true;
            }
        }
    }
    return false;
}

void Eval::initEvalMasks()
{

    for (int sq = 0; sq < 8; ++sq) {
        Eval::FileBBMask[sq] = 0;
        Eval::RankBBMask[sq] = 0;
    }

    for (int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            int sq = r * 8 + f;
            Eval::FileBBMask[f] |= (1 << sq);
            Eval::RankBBMask[r] |= (1 << sq);
        }
    }

    for (int sq = 0; sq < 64; ++sq) {
        Eval::IsolatedMask[sq] = 0;
        Eval::WhitePassedMask[sq] = 0;
        Eval::BlackPassedMask[sq] = 0;
    }

    for (int sq = 0; sq < 64; ++sq) {
        int tsq = sq + 8;

        while (tsq < 64) {
            Eval::WhitePassedMask[sq] |= (1 << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while (tsq >= 0) {
            Eval::BlackPassedMask[sq] |= (1 << tsq);
            tsq -= 8;
        }

        if (Board::FilesBrd[SQ120(sq)] > FILE_A) {
            Eval::IsolatedMask[sq] |= Eval::FileBBMask[Board::FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while (tsq < 64) {
                Eval::WhitePassedMask[sq] |= (1 << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while (tsq >= 0) {
                Eval::BlackPassedMask[sq] |= (1 << tsq);
                tsq -= 8;
            }
        }

        if (Board::FilesBrd[SQ120(sq)] < FILE_H) {
            Eval::IsolatedMask[sq] |= Eval::FileBBMask[Board::FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while (tsq < 64) {
                Eval::WhitePassedMask[sq] |= (1 << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while (tsq >= 0) {
                Eval::BlackPassedMask[sq] |= (1 << tsq);
                tsq -= 8;
            }
        }
    }
}
