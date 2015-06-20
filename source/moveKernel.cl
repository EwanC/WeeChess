/* 
   GAME MOVE

   0000 0000 0000 0000 0000 0111 1111 -> From [0x7F]
   0000 0000 0000 0011 1111 1000 0000 -> To [>> 7, 0x7F]
   0000 0000 0011 1100 0000 0000 0000 -> Captured Piece [>> 14, 0xF]
   0000 0000 0100 0000 0000 0000 0000 -> En Passent capture? [0x40000]
   0000 0000 1000 0000 0000 0000 0000 -> Pawn Start? [0x80000]
   0000 1111 0000 0000 0000 0000 0000 -> Piece promoted to[>> 20, 0xF]
   0001 0000 0000 0000 0000 0000 0000 -> Castle? [0x1000000]

*/



// Directions that each piece can move in
int constant PceDir[13][8] = {    
	                            {0, 0, 0, 0, 0, 0, 0}, // ALL
                                {0, 0, 0, 0, 0, 0, 0}, // WP
                                {-8, -19, -21, -12, 8, 19, 21, 12}, // WN
                                {-9, -11, 11, 9, 0, 0, 0, 0}, // WB
                                {-1, -10, 1, 10, 0, 0, 0, 0}, // WR
                                {-1, -10, 1, 10, -9, -11, 11, 9}, // WQ
                                {-1, -10, 1, 10, -9, -11, 11, 9}, // WK
                                {0, 0, 0, 0, 0, 0, 0}, // WP
                                {-8, -19, -21, -12, 8, 19, 21, 12}, // WN
                                {-9, -11, 11, 9, 0, 0, 0, 0}, // WB
                                {-1, -10, 1, 10, 0, 0, 0, 0}, // WR
                                {-1, -10, 1, 10, -9, -11, 11, 9}, // WQ
                                {-1, -10, 1, 10, -9, -11, 11, 9}  //WK
                            };

// How many directinons each piece has
unsigned int constant NumDir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

// How many directinons each piece has
unsigned int constant isSlider[13] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0};


int constant FilesBrd[120] = {
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            };

int constant RanksBrd[120] = {
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
                              100,   1,   1,   1,   1,   1,   1,   1,   1, 100,
                              100,   2,   2,   2,   2,   2,   2,   2,   2, 100,
                              100,   3,   3,   3,   3,   3,   3,   3,   3, 100,
                              100,   4,   4,   4,   4,   4,   4,   4,   4, 100,
                              100,   5,   5,   5,   5,   5,   5,   5,   5, 100,
                              100,   6,   6,   6,   6,   6,   6,   6,   6, 100,
                              100,   7,   7,   7,   7,   7,   7,   7,   7, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                              100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                            };

enum Piece  { EMPTY =0 , wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum Rank   { RANK_1=0, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum Square {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum Colour { WHITE=0, BLACK, BOTH };

int constant PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };



// MACROS
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)
#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))


#define MFLAGEP 0x40000   // Move flag En Pass
#define MFLAGPS 0x80000   // Move flag pawn start

// Kernel
// Workgroup - Piece Type
// WorkItem - Individual Piece
// No score eval!
__kernel void moveKernel(
                            __global unsigned int* squares,
                            __global const int* side,
                            __global const int* pieces,
                            __global unsigned long* moves // Return Value
                        )
{

    int group_id = get_group_id(0); // Piece Type
    int global_id = get_global_id(0);
    int sq120 = squares[global_id];
    int myside = side[global_id];
    bool slider = isSlider[group_id];
    for (int index = 0; index < NumDir[group_id]; ++index) 
    {
        int dir = PceDir[group_id][index];
        int t_sq = sq120 + dir;

        while (!SQOFFBOARD(t_sq)) {

            // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
            if (pieces[t_sq] != EMPTY) {
                if (PieceCol[pieces[t_sq]] == (myside ^ 1))
                {
                    moves[global_id] =  MOVE(sq120, t_sq, pieces[t_sq], EMPTY, 0);
                }
                break;
            }
            moves[global_id] =  MOVE(sq120, t_sq, EMPTY, EMPTY, 0);

            if(slider) // Horrific performace
            	break;
            else
            	t_sq += dir;

        }
    }

}


__kernel void WhitePawnKernel(
                                __global unsigned int* squares,
                                __global const int* pieces,
                                __global int* enpass,
                                __global unsigned long* moves // Return Value
                             )
{

    int group_id = get_group_id(0); // Piece Type
    int global_id = get_global_id(0);
    int sq120 = squares[global_id];
    int enPas = enpass[0];

    const unsigned int sqInfront = sq120 + 10;

    // Check square infront is empty
    if (pieces[sqInfront] == EMPTY)
    {
        // Pawn can be promoted
        if (RanksBrd[sq120] == RANK_7) 
        {
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, wQ, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, wR, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, wB, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, wN, 0);
        }
        else 
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, EMPTY, 0);
  
        // Pawn hasn't moved yet, can move two squares
        if (RanksBrd[sq120] == RANK_2 && pieces[sq120 + 20] == EMPTY) {
             moves[global_id] =  MOVE(sq120, (sq120 + 20), EMPTY, EMPTY, MFLAGPS);
        }
    }

    // Oppostion piece diagonally to the right
    const unsigned int sqRight = sq120 + 9;
    if (!SQOFFBOARD(sqRight) && PieceCol[pieces[sqRight]] == BLACK) {

        // Pawn can be promoted
        if (RanksBrd[sq120] == RANK_7) {
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], wQ, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], wR, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], wB, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], wN, 0);
        }
        else
        	moves[global_id] = MOVE(sq120, sqRight, pieces[sqRight], EMPTY, 0);

    }
    
    // Oppostion piece diagonally to the left
    const unsigned int sqLeft = sq120 + 11;
    if (!SQOFFBOARD(sqLeft) && PieceCol[pieces[sqLeft]] == BLACK) {
        if (RanksBrd[sq120] == RANK_7) {
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], wQ, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], wR, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], wB, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], wN, 0);
        }
        else
        	moves[global_id] = MOVE(sq120, sqLeft, pieces[sqLeft], EMPTY, 0);
    }

    //Enpassanet right
    if (enPas != NO_SQ && sqRight == enPas) {
        moves[global_id] = MOVE(sq120, sqRight, EMPTY, EMPTY, MFLAGEP);
    }
    
    // Enpassanet left
    if (enPas != NO_SQ && sqLeft == enPas) {
        moves[global_id] = MOVE(sq120, sqLeft, EMPTY, EMPTY, MFLAGEP);
    }
}

__kernel void BlackPawnKernel(
                            __global unsigned int* squares,
                            __global const int* pieces,
                            __global const int* enpass,
                            __global unsigned long* moves // Return Value
                        )
{

    int group_id = get_group_id(0); // Piece Type
    int global_id = get_global_id(0);
    int sq120 = squares[global_id];
    int enPas = enpass[global_id];

    const unsigned int sqInfront = sq120 - 10;

    // Check square infront is empty
    if (pieces[sqInfront] == EMPTY)
    {
        // Pawn can be promoted
        if (RanksBrd[sq120] == RANK_2) 
        {
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, bQ, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, bR, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, bB, 0);
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, bN, 0);
        }
        else 
            moves[global_id] = MOVE(sq120, sqInfront, EMPTY, EMPTY, 0);
  
        // Pawn hasn't moved yet, can move two squares
        if (RanksBrd[sq120] == RANK_7 && pieces[sq120 - 20] == EMPTY) {
            moves[global_id] =  MOVE(sq120, (sq120 - 20), EMPTY, EMPTY, MFLAGPS);
        }
    }

    // Oppostion piece diagonally to the right
    const unsigned int sqRight = sq120 - 9;
    if (!SQOFFBOARD(sqRight) && PieceCol[pieces[sqRight]] == BLACK) {

        // Pawn can be promoted
        if (RanksBrd[sq120] == RANK_2) {
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], bQ, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], bR, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], bB, 0);
            moves[global_id] =  MOVE(sq120, sqRight, pieces[sqRight], bN, 0);
        }
        else
        	moves[global_id] = MOVE(sq120, sqRight, pieces[sqRight], EMPTY, 0);

    }
    
    // Oppostion piece diagonally to the left
    const unsigned int sqLeft = sq120 + 11;
    if (!SQOFFBOARD(sqLeft) && PieceCol[pieces[sqLeft]] == BLACK) {
        if (RanksBrd[sq120] == RANK_2) {
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], bQ, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft],bR, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], bB, 0);
            moves[global_id] =  MOVE(sq120, sqLeft, pieces[sqLeft], bN, 0);
        }
        else
        	moves[global_id] = MOVE(sq120, sqLeft, pieces[sqLeft], EMPTY, 0);
    }

    //Enpassanet right
    if (enPas != NO_SQ && sqRight == enPas) {
        moves[global_id] = MOVE(sq120, sqRight, EMPTY, EMPTY, MFLAGEP);
    }
    
    // Enpassanet left
    if (enPas != NO_SQ && sqLeft == enPas) {
        moves[global_id] = MOVE(sq120, sqLeft, EMPTY, EMPTY, MFLAGEP);
    }
}
