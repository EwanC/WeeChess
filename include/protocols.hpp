#ifndef PROTO_H
#define PROTO_H

class Board;
class SearchInfo;

enum Mode
{
    CONSOLEMODE = 0,
    UCIMODE,
    XBMODE,
    PERFTMODE
};

// UCI GUI protocol
namespace UCI
{
void UCILoop(Board& b, SearchInfo& info);
void ParsePosition(char* lineIn, Board& b);
void ParseGo(char* line, SearchInfo& info, Board& b);
void ReadInput(SearchInfo& info);
int InputWaiting();
}

// interact with engine through command line
namespace CLI
{
void consoleLoop(Board& b, SearchInfo& info);
void printHelp();
void printUsage();
}

// XBOARD/WINBoard GUI protocol
namespace XBoard
{
void XBoardLoop(Board& b, SearchInfo& info);
void PrintOptions();
bool checkResult(Board& b);
bool DrawMaterial(const Board& b);
int ThreeFoldRep(const Board& b);
}

#endif // PROTO_H
