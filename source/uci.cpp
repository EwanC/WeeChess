#include "uci.hpp"
#include "types.hpp"

#include "stdio.h"
#include <cstring>
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"

#define INPUTBUFFER 400 * 6
#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int UCI::InputWaiting()
{
  fd_set readfds;
  struct timeval tv;
  FD_ZERO (&readfds);
  FD_SET (fileno(stdin), &readfds);
  tv.tv_sec=0; tv.tv_usec=0;
  select(16, &readfds, 0, 0, &tv);

  return (FD_ISSET(fileno(stdin), &readfds));

}

void UCI::ReadInput( SearchInfo& info) 
{
    int bytes;
    char input[256] = "", *endc;

    if (InputWaiting()) {    
		info.stopped = true;
		do {
		  bytes=read(fileno(stdin),input,256);
		} while (bytes<0);
		endc = strchr(input,'\n');
		if (endc) *endc=0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))    {
			  info.quit = true;
			}
		}
		return;
    }
}

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void UCI::ParseGo(char* line, SearchInfo& info, Board& b) 
{
    
	int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
    char *ptr = NULL;
	info.timeset = false;
	
	if ((ptr = strstr(line,"infinite"))) {
		;
	} 
	
	if ((ptr = strstr(line,"binc")) && b.m_side == BLACK) {
		inc = atoi(ptr + 5);
	}
	
	if ((ptr = strstr(line,"winc")) && b.m_side == WHITE) {
		inc = atoi(ptr + 5);
	} 
	
	if ((ptr = strstr(line,"wtime")) && b.m_side == WHITE) {
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"btime")) && b.m_side == BLACK) {
		time = atoi(ptr + 6);
	} 
	  
	if ((ptr = strstr(line,"movestogo"))) {
		movestogo = atoi(ptr + 10);
	} 
	  
	if ((ptr = strstr(line,"movetime"))) {
		movetime = atoi(ptr + 9);
	}
	  
	if ((ptr = strstr(line,"depth"))) {
		depth = atoi(ptr + 6);
	} 
	
	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}
	
	info.starttime = Search::GetTimeMs();
	info.depth = depth;
	
	if(time != -1) {
		info.timeset = true;
		time /= movestogo;
		time -= 50;		
		info.stoptime = info.starttime + time + inc;
	} 
	
	if(depth == -1) {
		info.depth = MAXDEPTH;
	}
	
	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time,info.starttime,info.stoptime,info.depth,info.timeset);

	Search::SearchPosition(b, info);

}

// position fen fenstr
// position startpos
// ... moves e2e4 e7e5 b7b8q
void UCI::ParsePosition(char* lineIn, Board& b) {
	
	lineIn += 9;
    char *ptrChar = lineIn;
	
    if(strncmp(lineIn, "startpos", 8) == 0){
        b.parseFen(START_FEN);
    } else {
        ptrChar = strstr(lineIn, "fen");
        if(ptrChar == NULL) {
            b.parseFen(START_FEN);
        } else {
            ptrChar+=4;
            b.parseFen(ptrChar);
        }
    }
	
	ptrChar = strstr(lineIn, "moves");
	int move;
	
	if(ptrChar != NULL) {
        ptrChar += 6;
        while(*ptrChar) {
              move = parseMove(ptrChar, b);
			  if(move == 0) break;
			     MakeMove(b, move);
              b.m_ply=0;
              while(*ptrChar && *ptrChar!= ' ') ptrChar++;
              ptrChar++;
        }
    }
	b.printBoard();	
}

void UCI::GameLoop()
{
	// Buffer standard in and standard out
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);

   	char line[INPUTBUFFER];
    printf("id name WeeChess\n");
    printf("id author Ewan Crawford\n");
    printf("uciok\n");

    Board b;
    Board::initStaticMembers();
    
    SearchInfo info;

    while (true) {
		memset(&line[0], 0, sizeof(line)); // Set line to 0
        fflush(stdout);

        if (!fgets(line, INPUTBUFFER, stdin)) //get input from standard input
        continue;

        if (line[0] == '\n')
        continue;

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if (!strncmp(line, "position", 8)) {
            ParsePosition(line, b);
        } else if (!strncmp(line, "ucinewgame", 10)) {
            ParsePosition("position startpos\n", b);
        } else if (!strncmp(line, "go", 2)) {
            ParseGo(line, info, b);
        } else if (!strncmp(line, "quit", 4)) {
            info.quit = true;
            break;
        } else if (!strncmp(line, "uci", 3)) {
            printf("id name WeeChess\n");
            printf("id author Ewan Crawford\n");
            printf("uciok\n");
        }
		if(info.quit) break;
    }
	
}