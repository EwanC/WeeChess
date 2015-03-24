#ifndef SEARCH_H
#define SEARCH_H

#include "board.hpp"
#include "move.hpp"

enum Mode { CONSOLEMODE = 0, UCIMODE, XBMODE };

// Information about the current search
struct SearchInfo {

    int starttime; // Time search started
    int stoptime;  // Time search stopped
    int depth;     // depth to search to
    int depthset;
    bool timeset; // time search limit
    int movestogo;
    int infinite;   // set to true then, continue until gui sends stopped
    Mode gameMode;  // Protocol used for communicating with the user
    uint64_t nodes; // count of all nodes visited
    bool postThinking;
    bool quit;    // set to true to back out and delete state
    bool stopped; // set to true to back out keeping state

    float fh;  // fail high
    float fhf; // fail high first

    int nullCut; // Number of null moves pruned
};

namespace Search {

    bool isRepetition(
        const Board& b); // Check for repition of current board position in last fifty moves
    void CheckUp(SearchInfo& info); // .. check if time up, or interrupt from GUI
    int Quiescence(int alpha, int beta, Board& b, SearchInfo& info); // Reduces horizon effect
    int AlphaBeta(int alpha, int beta, int depth, Board& b, SearchInfo& info,
                  bool DoNull); // Alhpa beta search, returns board score
    void ClearForSearch(
        Board& b, SearchInfo& info); // Clears alphaBeta heuristics as well as seachinfo and PVTable
    void SearchPosition(Board& b,
                        SearchInfo& info); // Iterative deepening, searching with alpha beta
    void PickNextMove(
        std::vector<Move>::iterator move,
        MoveList& list); // Finds the best scoring move in the move list and swaps with current move
    int GetTimeMs();     // Returns current time.

} // end namespace

#endif // SEARCH_H