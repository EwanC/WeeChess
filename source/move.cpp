#include "move.hpp"
#include <algorithm>  // std::sort
#include <cassert>
#include <iostream>
#include "board.hpp"
#include "eval.hpp"
#include "ocl.hpp"
#include "search.hpp"

// Castle Permission Array
// All initally set to 0xF apart from a1,e1,h1,a8,e8, and h8
// Every time a piece is moved the permission &= castlePerm[form/to]
static const int CastlePerm[120] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                    15, 13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 7,  15, 15, 15, 3,  15, 15, 11, 15,
                                    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};

Move::Move() : m_move(0), m_score(0) {}

// Converts move into string for printing
std::string Move::moveString() const {
  std::string move_str;

  int ff = Board::FilesBrd[FROMSQ(m_move)];
  int rf = Board::RanksBrd[FROMSQ(m_move)];
  int ft = Board::FilesBrd[TOSQ(m_move)];
  int rt = Board::RanksBrd[TOSQ(m_move)];

  move_str.push_back('a' + ff);
  move_str.push_back('1' + rf);
  move_str.push_back('a' + ft);
  move_str.push_back('1' + rt);

  int promoted = PROMOTED(m_move);

  if (promoted) {
    char pchar = 'q';
    if (Board::PieceKnight[promoted]) {
      pchar = 'n';
    } else if (Board::PieceRookQueen[promoted] && !Board::PieceBishopQueen[promoted]) {
      pchar = 'r';
    } else if (!Board::PieceRookQueen[promoted] && Board::PieceBishopQueen[promoted]) {
      pchar = 'b';
    }

    move_str.push_back(pchar);
  }

  return move_str;
}

/*
 Movegen -> loop over all pieces
            -> slider loop in each direction add move
            ->addmove list->moves.push_back(move)

*/

// Generate all moves for a board position
void MoveList::genAllMoves(const Board& b) {
  OCL* ocl = OCL::getInstance();
  std::vector<Move> pawn_moves = ocl->RunPawnMoveKernel(b);
  m_move_vec.insert(m_move_vec.end(), pawn_moves.begin(), pawn_moves.end());

  if (b.m_side == WHITE) {
    // castling
    if (b.m_castling & WKCA) {
      if (b.m_board[F1] == EMPTY && b.m_board[G1] == EMPTY) {
        if (!b.isSquareAttacked(E1, BLACK) && !b.isSquareAttacked(F1, BLACK)) {
          addQuietMove(b, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA));
        }
      }
    }

    if (b.m_castling & WQCA) {
      if (b.m_board[D1] == EMPTY && b.m_board[C1] == EMPTY && b.m_board[B1] == EMPTY) {
        if (!b.isSquareAttacked(E1, BLACK) && !b.isSquareAttacked(D1, BLACK)) {
          addQuietMove(b, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA));
        }
      }
    }
  } else {
    // castling
    if (b.m_castling & BKCA) {
      if (b.m_board[F8] == EMPTY && b.m_board[G8] == EMPTY) {
        if (!b.isSquareAttacked(E8, WHITE) && !b.isSquareAttacked(F8, WHITE)) {
          addQuietMove(b, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA));
        }
      }
    }

    if (b.m_castling & BQCA) {
      if (b.m_board[D8] == EMPTY && b.m_board[C8] == EMPTY && b.m_board[B8] == EMPTY) {
        if (!b.isSquareAttacked(E8, WHITE) && !b.isSquareAttacked(D8, WHITE)) {
          addQuietMove(b, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA));
        }
      }
    }
  }

  std::vector<Move> piece_moves = ocl->RunPieceMoveKernel(b);

  m_move_vec.insert(m_move_vec.end(), piece_moves.begin(), piece_moves.end());
}

/*
   Generates all Capture moves
*/
void MoveList::genAllCaps(const Board& b) {
  OCL* ocl = OCL::getInstance();
  std::vector<Move> pawn_caps = ocl->RunPawnMoveKernel(b, true);
  m_move_vec.insert(m_move_vec.end(), pawn_caps.begin(), pawn_caps.end());
  std::vector<Move> piece_caps = ocl->RunPieceMoveKernel(b, true);
  m_move_vec.insert(m_move_vec.end(), piece_caps.begin(), piece_caps.end());
}

// add quiet move to move list
void MoveList::addQuietMove(const Board& b, uint32_t move) {
  Move m;
  m.m_move = move;

  if (b.m_searchKillers[0][b.m_ply] == move)
    m.m_score = 900000;
  else if (b.m_searchKillers[1][b.m_ply] == move)
    m.m_score = 800000;
  else
    m.m_score = b.m_searchHistory[b.m_board[FROMSQ(move)]][TOSQ(move)];

  m_move_vec.push_back(m);
}

// prints invidual moves in move list
void MoveList::printList() const {
  std::vector<Move>::const_iterator itr;
  unsigned int count = 0;
  for (itr = m_move_vec.begin(); itr != m_move_vec.end(); itr++) {
    std::cout << "Move " << count << ": " << std::hex << itr->m_move << std::dec << " score: " << itr->m_score
              << std::endl;
  }
}

// parses algebraic move
uint32_t MoveGen::parseMove(char* ptrChar, Board& b) {
  if (ptrChar[1] > '8' || ptrChar[1] < '1') return 0;
  if (ptrChar[3] > '8' || ptrChar[3] < '1') return 0;
  if (ptrChar[0] > 'h' || ptrChar[0] < 'a') return 0;
  if (ptrChar[2] > 'h' || ptrChar[2] < 'a') return 0;

  int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
  int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

  assert(!SQOFFBOARD(from) && !SQOFFBOARD(to));

  MoveList list;
  list.genAllMoves(b);

  std::vector<Move>::iterator itr;
  for (itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) {
    uint32_t move = itr->m_move;
    if (FROMSQ(move) == from && TOSQ(move) == to) {
      Piece promPce = static_cast<Piece>(PROMOTED(move));
      if (promPce != EMPTY) {
        if ((promPce == wR || promPce == bR) && ptrChar[4] == 'r') {
          return move;
        } else if ((promPce == wB || promPce == bP) && ptrChar[4] == 'b') {
          return move;
        } else if ((promPce == wQ || promPce == bQ) && ptrChar[4] == 'q') {
          return move;
        } else if ((promPce == wN || promPce == bN) && ptrChar[4] == 'n') {
          return move;
        }
        continue;
      }
      return move;
    }
  }

  return 0;
}

// clears piece from board at 120 sq
void MoveGen::clearPiece(const int sq, Board& b) {
  assert(!SQOFFBOARD(sq));

  Piece pce = b.m_board[sq];

  assert(pce >= wP && pce <= bK);

  Colour colour = Board::PieceCol[pce];

  HASH_PCE(pce, sq);

  b.m_board[sq] = EMPTY;
  b.m_material[colour] -= Board::PieceVal[pce];

  if (pce != wP && pce != bP) {
    if (Board::PieceMaj[pce]) {
      b.m_majPce[colour]--;
    } else {
      b.m_minPce[colour]--;
    }
  }

  CLRBIT(b.m_pList[pce], SQ64(sq));
}

// adds piece to the board at 120sq
void MoveGen::addPiece(const int sq, Board& b, const Piece pce) {
  assert(pce >= wP && pce <= bK);
  assert(!SQOFFBOARD(sq));

  Colour colour = Board::PieceCol[pce];

  HASH_PCE(pce, sq);

  b.m_board[sq] = pce;

  if (pce != wP && pce != bP) {
    if (Board::PieceMaj[pce]) {
      b.m_majPce[colour]++;
    } else {
      b.m_minPce[colour]++;
    }
  }

  SETBIT(b.m_pList[pce], SQ64(sq));

  b.m_material[colour] += Board::PieceVal[pce];
}

// Moves piece from a 120sq to another 120sq
void MoveGen::movePiece(const int from, const int to, Board& b) {
  assert(!SQOFFBOARD(to));

  Piece pce = b.m_board[from];
  Colour col = Board::PieceCol[pce];

  HASH_PCE(pce, from);
  b.m_board[from] = EMPTY;

  HASH_PCE(pce, to);
  b.m_board[to] = pce;

  CLRBIT(b.m_pList[pce], SQ64(from));
  SETBIT(b.m_pList[pce], SQ64(to));
}

// Modifies board according to move
bool MoveGen::makeMove(Board& b, int move) {
  assert(b.checkBoard());

  int from = FROMSQ(move);
  int to = TOSQ(move);
  Colour side = b.m_side;

  assert(!SQOFFBOARD(from));
  assert(!SQOFFBOARD(to));
  assert(side == WHITE || side == BLACK);
  assert(b.m_board[from] >= wP && b.m_board[from] <= bK);

  b.m_history[b.m_hisply].posHash = b.m_posHash;

  if (move & MFLAGEP) {
    if (side == WHITE) {
      MoveGen::clearPiece(to - 10, b);
    } else {
      MoveGen::clearPiece(to + 10, b);
    }
  } else if (move & MFLAGCA) {
    switch (to) {
      case C1:
        MoveGen::movePiece(A1, D1, b);
        break;
      case C8:
        MoveGen::movePiece(A8, D8, b);
        break;
      case G1:
        MoveGen::movePiece(H1, F1, b);
        break;
      case G8:
        MoveGen::movePiece(H8, F8, b);
        break;
      default:
        assert(false && "Make move failed");
        break;
    }
  }

  if (b.m_enPas != NO_SQ) {
    HASH_EP;
  }

  HASH_CA;

  b.m_history[b.m_hisply].move = move;
  b.m_history[b.m_hisply].fiftyMove = b.m_fiftyMove;
  b.m_history[b.m_hisply].enPas = b.m_enPas;
  b.m_history[b.m_hisply].castlingPerm = b.m_castling;

  b.m_castling &= CastlePerm[from];
  b.m_castling &= CastlePerm[to];
  b.m_enPas = NO_SQ;

  HASH_CA;

  int captured = CAPTURED(move);
  b.m_fiftyMove++;

  if (captured != EMPTY) {
    assert(captured >= wP && captured <= bK);
    MoveGen::clearPiece(to, b);
    b.m_fiftyMove = 0;
  }

  b.m_hisply++;
  b.m_ply++;

  if (b.m_board[from] == wP || b.m_board[from] == bP) {
    b.m_fiftyMove = 0;
    if (move & MFLAGPS) {
      if (side == WHITE) {
        b.m_enPas = static_cast<Square>(from + 10);
        assert(Board::RanksBrd[b.m_enPas] == RANK_3);
      } else {
        b.m_enPas = static_cast<Square>(from - 10);
        assert(Board::RanksBrd[b.m_enPas] == RANK_6);
      }
      HASH_EP;
    }
  }

  MoveGen::movePiece(from, to, b);

  Piece prPce = static_cast<Piece>(PROMOTED(move));
  if (prPce != EMPTY) {
    assert(prPce != bP && prPce > wP && prPce < bK);
    MoveGen::clearPiece(to, b);
    MoveGen::addPiece(to, b, prPce);
  }

  if (Board::PieceKing[b.m_board[to]]) {
    b.m_kingSq[b.m_side] = static_cast<Square>(to);
  }

  b.m_side = static_cast<Colour>(b.m_side ^ 1);
  HASH_SIDE;

  assert(b.checkBoard());

  if (b.isSquareAttacked(b.m_kingSq[side], b.m_side)) {
    MoveGen::takeMove(b);
    return false;
  }

  return true;
}

// Takes back last move
void MoveGen::takeMove(Board& b) {
  assert(b.checkBoard());

  b.m_hisply--;
  b.m_ply--;

  int move = b.m_history[b.m_hisply].move;
  int from = FROMSQ(move);
  int to = TOSQ(move);

  assert(!SQOFFBOARD(from));
  assert(!SQOFFBOARD(to));

  if (b.m_enPas != NO_SQ) {
    HASH_EP;
  }
  HASH_CA;

  b.m_castling = b.m_history[b.m_hisply].castlingPerm;
  b.m_fiftyMove = b.m_history[b.m_hisply].fiftyMove;
  b.m_enPas = b.m_history[b.m_hisply].enPas;

  if (b.m_enPas != NO_SQ) {
    HASH_EP;
  }

  HASH_CA;

  b.m_side = static_cast<Colour>(b.m_side ^ 1);
  HASH_SIDE;

  if (MFLAGEP & move) {
    if (b.m_side == WHITE) {
      MoveGen::addPiece(to - 10, b, bP);
    } else {
      MoveGen::addPiece(to + 10, b, wP);
    }
  } else if (MFLAGCA & move) {
    switch (to) {
      case C1:
        MoveGen::movePiece(D1, A1, b);
        break;
      case C8:
        MoveGen::movePiece(D8, A8, b);
        break;
      case G1:
        MoveGen::movePiece(F1, H1, b);
        break;
      case G8:
        MoveGen::movePiece(F8, H8, b);
        break;
      default:
        assert(false && "Take move failed");
        break;
    }
  }

  MoveGen::movePiece(to, from, b);

  if (Board::PieceKing[b.m_board[from]]) {
    b.m_kingSq[b.m_side] = static_cast<Square>(from);
  }

  Piece captured = static_cast<Piece>(CAPTURED(move));
  if (captured != EMPTY) {
    assert(captured >= wP && captured <= bK);
    MoveGen::addPiece(to, b, captured);
  }

  if (PROMOTED(move) != EMPTY) {
    Piece prPce = static_cast<Piece>(PROMOTED(move));
    assert(prPce != bP && b.m_board[from] > wP && b.m_board[from] <= bK);

    MoveGen::clearPiece(from, b);
    MoveGen::addPiece(from, b, (Board::PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
  }

  assert(b.checkBoard());
}

// Checks if move is valid
// Used for GetPVLine to check move at a given depth
bool MoveGen::moveExists(Board& b, const int move) {
  MoveList list;
  list.genAllMoves(b);
  std::vector<Move>::iterator itr;
  for (itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) {
    if (!MoveGen::makeMove(b, itr->m_move)) {
      continue;
    }
    MoveGen::takeMove(b);
    if (itr->m_move == move) {
      return true;
    }
  }
  return false;
}

// Pass on move if not in check
void MoveGen::makeNullMove(Board& b) {
  assert(b.checkBoard());
  Colour opposition = static_cast<Colour>(b.m_side ^ 1);
  assert(!b.isSquareAttacked(b.m_kingSq[b.m_side], opposition));

  b.m_ply++;
  b.m_history[b.m_hisply].posHash = b.m_posHash;

  if (b.m_enPas != NO_SQ) HASH_EP;

  b.m_history[b.m_hisply].move = 0;
  b.m_history[b.m_hisply].fiftyMove = b.m_fiftyMove;
  b.m_history[b.m_hisply].enPas = b.m_enPas;
  b.m_history[b.m_hisply].castlingPerm = b.m_castling;
  b.m_enPas = NO_SQ;

  b.m_side = opposition;
  b.m_hisply++;
  HASH_SIDE;

  assert(b.checkBoard());
  assert(b.m_hisply >= 0 && b.m_hisply < MAX_GAME_MOVES);
  assert(b.m_ply >= 0 && b.m_ply < MAXDEPTH);

  return;
}

// Undo null move
void MoveGen::takeNullMove(Board& b) {
  assert(b.checkBoard());

  b.m_hisply--;
  b.m_ply--;

  if (b.m_enPas != NO_SQ) HASH_EP;

  b.m_castling = b.m_history[b.m_hisply].castlingPerm;
  b.m_fiftyMove = b.m_history[b.m_hisply].fiftyMove;
  b.m_enPas = b.m_history[b.m_hisply].enPas;

  if (b.m_enPas != NO_SQ) HASH_EP;

  b.m_side = static_cast<Colour>(b.m_side ^ 1);
  HASH_SIDE;

  assert(b.checkBoard());
  assert(b.m_hisply >= 0 && b.m_hisply < MAX_GAME_MOVES);
  assert(b.m_ply >= 0 && b.m_ply < MAXDEPTH);
}

std::string MoveGen::moveString(uint32_t move) {
  std::string move_str;

  int ff = Board::FilesBrd[FROMSQ(move)];
  int rf = Board::RanksBrd[FROMSQ(move)];
  int ft = Board::FilesBrd[TOSQ(move)];
  int rt = Board::RanksBrd[TOSQ(move)];

  move_str.push_back('a' + ff);
  move_str.push_back('1' + rf);
  move_str.push_back('a' + ft);
  move_str.push_back('1' + rt);

  int promoted = PROMOTED(move);

  if (promoted) {
    char pchar = 'q';
    if (Board::PieceKnight[promoted]) {
      pchar = 'n';
    } else if (Board::PieceRookQueen[promoted] && !Board::PieceBishopQueen[promoted]) {
      pchar = 'r';
    } else if (!Board::PieceRookQueen[promoted] && Board::PieceBishopQueen[promoted]) {
      pchar = 'b';
    }

    move_str.push_back(pchar);
  }

  return move_str;
}
