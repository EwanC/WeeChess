#ifndef PERFT_H
#define PERFT_H

bool runPerft(Board& b,const int depth);
uint32_t Perft(const int depth, Board& b);
uint32_t PerftTest(const int depth, Board& b, bool verbose = false);


#endif //PERFT_H