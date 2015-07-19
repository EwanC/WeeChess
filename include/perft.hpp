#ifndef PERFT_H
#define PERFT_H

// Run perftest to defined depth
bool runPerft(Board& b, const int depth);

// Run pert test on single board
uint32_t Perft(const int depth, Board& b, const bool verbose);

// Run individual perft test
uint32_t PerftTest(const int depth, Board& b, bool verbose = false);

#endif // PERFT_H