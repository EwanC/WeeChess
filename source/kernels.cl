int constant BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};
// TODO NEED TO REVERSE BLACK TABLES
int constant BitMasks1[13][64] = { //Piece Tables
                                  { // All
                                    0,   0,   0,   0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                  },
                                  { // White PawnTable
                                    0,  0,  0,  0,  0,  0,  0,  0,  10, 10, 0,  -10, -10, 0,  10, 10,
                                    5,  0,  0,  5,  5,  0,  0,  5,  0,  0,  10, 20,  20,  10, 0,  0,
                                    5,  5,  5,  10, 10, 5,  5,  5,  10, 10, 10, 20,  20,  10, 10, 10,
                                    20, 20, 20, 30, 30, 20, 20, 20, 0,  0,  0,  0,   0,   0,  0,  0
                                  },
                                  { // White KnightTable
                                    0, -10, 0,  0,  0,  0,  -10, 0, 0, 0,  0,  5,  5,  0,  0,  0,
                                    0, 0,   10, 10, 10, 10, 0,   0, 0, 0,  10, 20, 20, 10, 5,  0,
                                    5, 10,  15, 20, 20, 15, 10,  5, 5, 10, 10, 20, 20, 10, 10, 5,
                                    0, 0,   5,  10, 10, 5,  0,   0, 0, 0,  0,  0,  0,  0,  0,  0
                                  },
                                  { // White Bishop Table
                                    0, 0,  -10, 0,  0,  -10, 0,  0, 0, 0,  0,  10, 10, 0,  0,  0,
                                    0, 0,  10,  15, 15, 10,  0,  0, 0, 10, 15, 20, 20, 15, 10, 0,
                                    0, 10, 15,  20, 20, 15,  10, 0, 0, 0,  10, 15, 15, 10, 0,  0,
                                    0, 0,  0,   10, 10, 0,   0,  0, 0, 0,  0,  0,  0,  0,  0,  0
                                  },
                                  { // White Rook Table
                                    0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                                    0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                                    0,  0,  5,  10, 10, 5,  0,  0,  0, 0, 5, 10, 10, 5, 0, 0,
                                    25, 25, 25, 25, 25, 25, 25, 25, 0, 0, 5, 10, 10, 5, 0, 0
                                  },
                                  { // White Queen 
                                    0,   0,   0,   0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                  },
                                  { // White King Opening Table
                                    0,   5,   5,   -10, -10, 0,   10,  5,   -30, -30, -30, -30, -30,
                                    -30, -30, -30, -50, -50, -50, -50, -50, -50, -50, -50, -70, -70,
                                    -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70,
                                    -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70,
                                    -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70, -70
                                  },
                                  { // Black PawnTable
                          
                                      0,   0,   0,   0,   0,   0,   0,   0,
                                    -20, -20, -20, -30, -30, -20, -20, -20, 
                                    -10, -10, -10, -20, -20, -10, -10, -10,
                                     -5,  -5,  -5, -10, -10,  -5,  -5,  -5,
                                      0,   0, -10, -20, -20, -10,   0,   0,
                                     -5,   0,   0,  -5,  -5,   0,   0,  -5,
                                    -10, -10,   0,  10,  10,   0, -10, -10,
                                      0,   0,   0,   0,   0,   0,   0,   0
                                  },
                                  { // Black KnightTable
                                
                                    0,   0,   0,   0,   0,   0,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                   -5, -10, -10, -20, -20, -10, -10, -5,
                                   -5, -10, -15, -20, -20, -15, -10, -5,
                                    0,   0, -10, -20, -20, -10,  -5,  0,
                                    0,   0, -10, -10, -10, -10,   0,  0,
                                    0,   0,   0,  -5,  -5,   0,   0,  0,
                                    0,  10,   0,   0,   0,   0,  10,  0
                                 },  
                                 { // Black Bishop Table
                                     
                                  0,   0,  0,    0,   0,   0,   0, 0,
                                  0,   0,  0,  -10, -10,   0,   0, 0,
                                  0,   0, -10, -15, -15, -10,   0, 0,
                                  0, -10, -15, -20, -20, -15, -10, 0,
                                  0, -10, -15, -20, -20, -15, -10, 0,
                                  0,   0, -10, -15, -15, -10,   0, 0,
                                  0,   0,   0, -10, -10,   0,   0, 0,
                                  0,   0,  10,   0,   0,  10,   0, 0
                                 },
                                  
                                  { // Black Rook Table
                                    
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                  -25, -25, -25, -25, -25, -25, -25, -25,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0
                                 },
                                  { // Black Queen 
                                    0,   0,   0,   0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                  },
                                  { // Black King Opening Table
                                    0,   -5,   -5,   10, 10, 0,   -10,  -5,   30, 30, 30, 30, 30,
                                    30, 30, 30, 50, 50, 50, 50, 50, 50, 50, 50, 70, 70,
                                    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
                                    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
                                    70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70
                                  }
                                };

unsigned long constant WhitePassedMask[64] = 
                        {  // White pawn passed masks
                           0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                           0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                           0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                           0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                           0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                           0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                           0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                           0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                           0x0000000003030300, 0x0000000007070700, 0x000000000e0e0e00, 0x000000001c1c1c00,
                           0x0000000038383800, 0x0000000070707000, 0xffffffffe0e0e000, 0xffffffffc0c0c000,
                           0x0000000003030000, 0x0000000007070000, 0x000000000e0e0000, 0x000000001c1c0000,
                           0x0000000038380000, 0x0000000070700000, 0xffffffffe0e00000, 0xffffffffc0c00000,
                           0x0000000003000000, 0x0000000007000000, 0x000000000e000000, 0x000000001c000000,
                           0x0000000038000000, 0x0000000070000000, 0xffffffffe0000000, 0xffffffffc0000000,
                           0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
                           0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
                        };

unsigned long constant BlackPassedMask[64] = 
                        {   // Black pawn passed masks
                            0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
                            0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
                            0x0000000000000003, 0x0000000000000007, 0x000000000000000e, 0x000000000000001c,
                            0x0000000000000038, 0x0000000000000070, 0x00000000000000e0, 0x00000000000000c0,
                            0x0000000000000303, 0x0000000000000707, 0x0000000000000e0e, 0x0000000000001c1c,
                            0x0000000000003838, 0x0000000000007070, 0x000000000000e0e0, 0x000000000000c0c0,
                            0x0000000000030303, 0x0000000000070707, 0x00000000000e0e0e, 0x00000000001c1c1c,
                            0x0000000000383838, 0x0000000000707070, 0x0000000000e0e0e0, 0x0000000000c0c0c0,
                            0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                            0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                            0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                            0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                            0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                            0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0,
                            0x0000000003030303, 0x0000000007070707, 0x000000000e0e0e0e, 0x000000001c1c1c1c,
                            0x0000000038383838, 0x0000000070707070, 0xffffffffe0e0e0e0, 0xffffffffc0c0c0c0
                        };

unsigned long constant FileMask[8] =
                        {
                            0x0000000001010101, 0x0000000002020202, 0x0000000004040404, 0x0000000008080808, 
                            0x0000000010101010, 0x0000000020202020, 0x0000000040404040, 0xffffffff80808080
                        };

unsigned long constant RankMask[8] =  
                        {             
                            0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0xffffffffff000000, 
                            0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0xffffffffff000000
                        };

unsigned long constant IsolatedMask[64] = 
                        {   // Pawn Isolated masks
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040
                        };


inline int popBit(unsigned long* bb)
 {
  unsigned long b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned int) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

__kernel void evalKernel(
                         __global unsigned long* bitboards,
                         __global int* score,
                         __local int* local_score
                        )
{
    int i = get_group_id(0);
    int j = get_local_id(0);

    unsigned long board = bitboards[i*13 + j];
    local_score[0] = 0;
    int priv_score = 0;
    barrier(CLK_LOCAL_MEM_FENCE);


    while(board != 0)
    {
        int sq64 = popBit(&board);
        priv_score += BitMasks1[j][sq64];
        //priv_score += BitMasks2[j][sq64];
        //priv_score += BitMasks3[j][sq64];
    }

    atomic_add(local_score, priv_score);
    barrier(CLK_LOCAL_MEM_FENCE);


    if(j == 0)
       score[i] = local_score;
}