#ifndef UCI_H
#define UCI_H

#include "board.hpp"
#include "search.hpp"

namespace UCI {

void GameLoop();
void ParsePosition(char* lineIn, Board& b);
void ParseGo(char* line, SearchInfo& info, Board& b);
void ReadInput( SearchInfo& info);
int InputWaiting();

}


#endif //UCI_H