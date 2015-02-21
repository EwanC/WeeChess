#ifndef PERFT_H
#define PERFT_H

void runPerft(Board& b);
uint32_t Perft(const int depth, Board& b);
void PerftTest(const int depth, Board& b, bool verbose = false);


#endif //PERFT_H