__kernel void evalKernel(
                         __global int* bitboards,
                         __global int* score,
                         __local int* local_score
                        )
{
    int i = get_group_id(0);
    int j = get_local_id(0);

    int board = bitboards[i*13 + j];

    int priv_score;\
    for(priv_score = 0; board; priv_score++, board &= board - 1);

    atomic_add(local_score, priv_score);
    barrier (CLK_LOCAL_MEM_FENCE);


    if(j == 0)
       score[i] = local_score;
}