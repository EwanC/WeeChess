#ifndef PROTO_H
#define PROTO_H

#include "board.hpp"
#include "search.hpp"

namespace UCI {

void UCILoop(Board& b, SearchInfo& info);
void ParsePosition(char* lineIn, Board& b);
void ParseGo(char* line, SearchInfo& info, Board& b);
void ReadInput( SearchInfo& info);
int InputWaiting();

}

void consoleLoop(Board& b, SearchInfo& info);

namespace XBoard{
    void XBoardLoop(Board& b, SearchInfo& info);
    void PrintOptions();
    bool checkResult(Board& b);
    bool DrawMaterial(const Board& b);
    int ThreeFoldRep(const Board& b);
}

#endif //PROTO_H