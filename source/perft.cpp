#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "move.hpp"
#include "perft.hpp"
#include "board.hpp"

// Run perftest to defined depth
bool runPerft(Board& b, const int depth)
{
    std::ifstream infile("../perftsuite.epd"); // Input file of perft cases
    std::string line;

    int count = 0;
    bool pass = true;

    assert(depth > 0 && depth < 7);
    unsigned short passes = 0;
    unsigned short tests = 0;
    while (std::getline(infile, line)) {

        b.parseFen(line.c_str());

        std::size_t pos = 0; // expected leaf nodes

        for (int i = 0; i < depth; ++i) {
            pos = line.find(";", pos + 1); // expected leaf nodes
        }

        int expected = atoi((line.substr(pos + 3)).c_str());

        uint32_t result = PerftTest(depth, b, true);
        std::cout << "\nTest " << std::dec << count++ << " to depth " << depth;
        std::cout << " ===> expected: " << expected << ", calculated " << result;

        if (expected == result) {
            std::cout << " PASS\n";
            ++passes;
        }
        else {
            std::cout << " FAIL\n";
            pass = false;
        }
        ++tests;
    }

    std::cout << "\nPassed "<<passes<< "/"<<tests<<std::endl;
    if (pass)
        std::cout << "\nPerft Success\n";
    else
        std::cout << "\nPerft Failure\n";

    return pass;
}

// Run pert test on single board
uint32_t Perft(const int depth, Board& b)
{

    assert(b.checkBoard());

    if (depth == 0) {
        return 1;
    }

    MoveList list;
    list.genAllMoves(b);

    std::vector<Move>::iterator itr;

    int nodesAccum = 0;
    for (itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) {

        if (!MoveGen::makeMove(b, itr->m_move)) {
            continue;
        }
        nodesAccum += Perft(depth - 1, b);
        MoveGen::takeMove(b);
    }

    return nodesAccum;
}

// Run individual perft test
uint32_t PerftTest(const int depth, Board& b, bool verbose)
{

    assert(b.checkBoard());

    if (verbose)
        b.printBoard();

    uint32_t leafNodes = 0;

    MoveList list;
    list.genAllMoves(b);

    std::vector<Move>::iterator itr;
    int moveNum = 0;
    for (itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); itr++) {

        if (verbose)
            std::cout <<std::dec << "move " << moveNum++ << " : " << itr->moveString()<<std::endl;

        uint32_t move = itr->m_move;
        if (!MoveGen::makeMove(b, move)) {
            continue;
        }
        uint32_t calcNodes = Perft(depth - 1, b);
        leafNodes += calcNodes;
        MoveGen::takeMove(b);
    }

    return leafNodes;
}
