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
unsigned int contant NumDir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

// How many directinons each piece has
unsigned int contant isSlider[13] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0};


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

#define BOTH 2 
#define WHITE 0
#define BLACK 1
int constant PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };



// MACROS
#define OFFBOARD 100
#define EMPTY 0
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
                            __global unsigned long* moves, // Return Value
                        )
{

    int group_id = get_group_id(0); // Piece Type
    int global_id = get_global_id(0);
    int sq120 = squares[global_id];
  
    for (int index = 0; index < NumDir[group_id]; ++index) 
    {
        int dir = PceDir[group_id][index];
        int t_sq = sq120 + dir;

        while (!SQOFFBOARD(t_sq)) {

            // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
            if (pieces[t_sq] != EMPTY) {
                if (PieceCol[pieces[t_sq]] == (side ^ 1))
                {
                    moves[global_id] =  MOVE(sq120, t_sq, pieces[t_sq], EMPTY, 0);
                }
                break;
            }
            moves[global_id] =  MOVE(sq120, t_sq, EMPTY, EMPTY, 0));

            if(isSlider[group_id]) // Horrific performace
            	break;
            else
            	t_sq += dir;

        }
    }

}

__kernel void pawnKernel(
                            __global unsigned int* squares,
                            __global const int* side,
                            __global const int* pieces,
                            __global unsigned long* moves, // Return Value
                        )
{

    int group_id = get_group_id(0); // Piece Type
    int global_id = get_global_id(0);
    int sq120 = squares[global_id];


    // JUST FOR WHITE SIDE AT THE MO, NEED TO DO BOTH COLOURS

    if (pieces[sq120 + 10] == EMPTY)
    {
               
        if (Board::RanksBrd[sq120] == RANK_7 && side == WHITE) 
        {
            moves[global_id] = MOVE(sq120, sq120 + 10, EMPTY, wQ, 0);
            moves[global_id] = MOVE(sq120, sq120 + 10, EMPTY, wR, 0);
            moves[global_id] = MOVE(sq120, sq120 + 10, EMPTY, wB, 0);
            moves[global_id] = MOVE(sq120, sq120 + 10, EMPTY, wN, 0);
        }
        else 
            moves[global_id] = MOVE(from, to, EMPTY, EMPTY, 0);
  
        if (RanksBrd[sq120] == RANK_2 && pieces[sq120 + 20] == EMPTY) {
             moves[global_id] =  MOVE(sq120, (sq120 + 20), EMPTY, EMPTY, MFLAGPS);
        }
    }

  
    if (!SQOFFBOARD(sq120 + 9) && PieceCol[pieces[sq120 + 9]] == BLACK) {

        if (Board::RanksBrd[sq120] == RANK_7 && side == WHITE) {
            moves[global_id] =  MOVE(sq120, sq120 + 9, pieces[sq120 + 9], wQ, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 9, pieces[sq120 + 9], wR, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 9, pieces[sq120 + 9], wB, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 9, pieces[sq120 + 9], wN, 0);
        }
        else
        	 moves[global_id] = MOVE(sq120, sq120 + 9, pieces[sq120 + 9], EMPTY, 0);

    }
            
    if (!SQOFFBOARD(sq120 + 11) && PieceCol[pieces[sq120 + 11]] == BLACK) {
        if (Board::RanksBrd[sq120] == RANK_7 && side == WHITE) {
            moves[global_id] =  MOVE(sq120, sq120 + 11, pieces[sq120 + 11], wQ, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 11, pieces[sq120 + 11], wR, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 11, pieces[sq120 + 11], wB, 0);
            moves[global_id] =  MOVE(sq120, sq120 + 11, pieces[sq120 + 11], wN, 0);
        }
        else
        	addCaptureMove(b, MOVE(sq120, sq120 + 9, pieces[sq120 + 9], EMPTY, 0);    
    }

    if (b.m_enPas != NO_SQ && sq120 + 9 == b.m_enPas) {
        moves[global_id] = MOVE(sq120, sq120 + 9, EMPTY, EMPTY, MFLAGEP);
    }
    
    if (b.m_enPas != NO_SQ && sq120 + 11 == b.m_enPas) {
        moves[get_global_id] = MOVE(sq120, sq120 + 11, EMPTY, EMPTY, MFLAGEP);
    }

}