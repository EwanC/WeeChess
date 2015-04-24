int constant BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

int constant PawnTable[64] = {0,  0,  0,  0,  0,  0,  0,  0,  10, 10, 0,  -10, -10, 0,  10, 10,
                               5,  0,  0,  5,  5,  0,  0,  5,  0,  0,  10, 20,  20,  10, 0,  0,
                               5,  5,  5,  10, 10, 5,  5,  5,  10, 10, 10, 20,  20,  10, 10, 10,
                               20, 20, 20, 30, 30, 20, 20, 20, 0,  0,  0,  0,   0,   0,  0,  0};



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

    int priv_score = 0;

    while(board != 0)
    {
        int sq64 = popBit(&board);
        priv_score += PawnTable[sq64];

    }

    atomic_add(local_score, priv_score);
    barrier (CLK_LOCAL_MEM_FENCE);


    if(j == 0)
       score[i] = local_score;
}