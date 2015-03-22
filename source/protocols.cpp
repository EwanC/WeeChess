#include "protocols.hpp"
#include "types.hpp"

#include <cstring>
#include <iostream>
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"

#define INPUTBUFFER 400 * 6 // Input buffer size
#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" // Initial pos FEN

// Returns true if data waiting to be read from stdin
int UCI::InputWaiting()
{
    fd_set readfds;
    struct timeval tv;
  
    FD_ZERO (&readfds);
    FD_SET (fileno(stdin), &readfds);
   
    tv.tv_sec=0;
    tv.tv_usec=0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));

}

// Read data from stdin
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
		if (endc) 
			*endc=0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))    {
			    info.quit = true;
			}
		}
		return;
    }
}

// Search position
void UCI::ParseGo(char* line, SearchInfo& info, Board& b) 
{
    
    // Setup search info
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
	
	// set search time
	if(time != -1) {
		info.timeset = true;
		time /= movestogo;
		time -= 50;		
		info.stoptime = info.starttime + time + inc;
	} 
	
	if(depth == -1) {
		info.depth = MAXDEPTH;
	}
	
	// Search position
	std::cout << "time:"<<time;
	std::cout << " start:"<<info.starttime; 
	std::cout << " stop:" <<info.stoptime;
	std::cout << " depth:"<<info.depth;
	std::cout <<" timeset:"<<info.timeset<<std::endl;

	Search::SearchPosition(b, info);

}

// ParseFen string or move string
void UCI::ParsePosition(char* lineIn, Board& b) {
	
	lineIn += 9;
    char *ptrChar = lineIn;
	
	// set board to fen
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
	
	// Make move
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

// Loop of UCI GUI protocol
void UCI::UCILoop(Board& b, SearchInfo& info)
{
	// Buffer standard in and standard out
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // Chess engine id
   	char line[INPUTBUFFER];
    std::cout<<"id name WeeChess\n";
    std::cout<<"id author Ewan Crawford\n";
    std::cout<<"uciok\n";


    while (true) {
		memset(&line[0], 0, sizeof(line)); // Set line to 0
        fflush(stdout);

        if (!fgets(line, INPUTBUFFER, stdin)) //get input from standard input
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7)) {
            std::cout<<"readyok\n";
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
            std::cout<<"id name WeeChess\n";
            std::cout<<"id author Ewan Crawford\n";
            std::cout<<"uciok\n";
        }
		if(info.quit) 
			break;
    }
	
}

void CLI::printUsage()
{
	std::cout << "Usage: WeeChess [--help|-h] [--xboard|-x] [--uci|-u] [--cli|-c]\n";
    std::cout << "--help        Displays this mesage.\n";
    std::cout << "--cli         Interact with WeeChess thorugh command line.\n";
    std::cout << "--xboard      WeeChess uses the Xboard protocol.\n";
    std::cout << "--uci         WeeChess uses UCI protocol. Default if no option is specified.\n";

}

void CLI::printHelp()
{

    std::cout<<"CLI Commands:\n";
	std::cout<<"quit - quit game\n";
	std::cout<<"force - computer will not think\n";
	std::cout<<"print - show board\n";
	std::cout<<"post - show thinking\n";
	std::cout<<"nopost - do not show thinking\n";
	std::cout<<"new - start new game\n";
	std::cout<<"go - set computer thinking\n";
	std::cout<<"depth x - set depth to x\n";
	std::cout<<"time x - set thinking time to x seconds (depth still applies if set)\n";
	std::cout<<"view - show current depth and movetime settings\n";
	std::cout<<"setboard x - set position to fen x\n";
	std::cout<<"** note ** - to reset time and depth, set to 0\n";
	std::cout<<"enter moves using b7b8q notation\n\n\n";
}

// Interact with WeeChess through the console
void CLI::consoleLoop(Board& b, SearchInfo& info)
{

	std::cout<<"Welcome to WeeChess Console Mode!\n";
	std::cout<<"Type help for commands\n\n";

	info.gameMode = CONSOLEMODE;
	info.postThinking = true;
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	int depth = MAXDEPTH, movetime = 3000;            
	int engineSide = BLACK;    
	int move = 0;		
	char inBuf[80], command[80];	
	std::string input_buffer;
	
	b.parseFen(START_FEN);	// Setup initial board position
	
	while(true) { 

		fflush(stdout);

        // get Chess engine to search position and make move
		if(b.m_side == engineSide && XBoard::checkResult(b) == false) {  
			info.starttime = Search::GetTimeMs();
			info.depth = depth;
			
			if(movetime != 0) {
				info.timeset = true;
				info.stoptime = info.starttime + movetime;
			} 	
			
			Search::SearchPosition(b, info);
		}	
		
		// Get user input
		std::cout<<"\nWeeChess > ";

		fflush(stdout); 
	
		memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);
		if (!fgets(inBuf, 80, stdin))
		    continue;
    
		sscanf(inBuf, "%s", command);
		
		// Print help for used
		if(!strcmp(command, "help")) { 
			CLI::printHelp();
			continue;
		}

		if(!strcmp(command, "setboard")){
			engineSide = BOTH;
			b.parseFen(inBuf+9);
			continue;
		}
    
		if(!strcmp(command, "quit")) { 
			info.quit = true;
			break; 
		}
		
		if(!strcmp(command, "post")) { 
			info.postThinking = true;
			continue; 
		}
		
		if(!strcmp(command, "print")) { 
			b.printBoard();
			continue; 
		}
		
		if(!strcmp(command, "nopost")) { 
			info.postThinking = false;
			continue; 
		}
		
		if(!strcmp(command, "force")) { 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(!strcmp(command, "view")) {
			if(depth == MAXDEPTH) 
				std::cout << " depth not set ";
			else 
				std::cout << " depth "<<depth;
			
			if(movetime != 0) 
				std::cout <<" movetime " << movetime/1000 << "s\n";
			else 
				std::cout <<" movetime not set\n";
			
			continue; 
		}
		
		if(!strcmp(command, "depth")) {
			sscanf(inBuf, "depth %d", &depth); 
		    if(depth==0) 
		    	depth = MAXDEPTH;
			continue; 
		}
		
		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &movetime); 
			movetime *= 1000;
			continue; 
		} 
		
		if(!strcmp(command, "new")) { 
			engineSide = BLACK; 
			b.parseFen(START_FEN);
			continue; 
		}
		
		if(!strcmp(command, "go")) { 
			engineSide = b.m_side;  
			continue; 
		}	
		
		// Get Move from user
		move = parseMove(inBuf, b);	
		if(move == 0) { // Invalud move
			std::cout << "Command unknown:"<<inBuf<<std::endl;
			continue;
		}
		MakeMove(b, move); // make user move
		b.m_ply=0;
    }	
}

// Check for three fold repition
int XBoard::ThreeFoldRep(const Board& b) {
	int r = 0;
	for (int i = 0; i < b.m_hisply; ++i)	{
	    if (b.m_history[i].posHash == b.m_posHash) {
		    r++;
		}
	}
	return r;
}

// Returns true if both sides are drawn in material value
bool XBoard::DrawMaterial(const Board& b) 
{
    if (Bitboard::countBits(b.m_pList[wP]) || Bitboard::countBits(b.m_pList[bP]))
        return false;

    if (Bitboard::countBits(b.m_pList[wQ]) || Bitboard::countBits(b.m_pList[bQ]) 
    	|| Bitboard::countBits(b.m_pList[wR]) || Bitboard::countBits(b.m_pList[bR])) 
        return false;

    if (Bitboard::countBits(b.m_pList[wB]) > 1 || Bitboard::countBits(b.m_pList[bB])  > 1) 
        return false;

    if (Bitboard::countBits(b.m_pList[wN]) > 1 || Bitboard::countBits(b.m_pList[bN]) > 1) 
    	return false;

    if (Bitboard::countBits(b.m_pList[wN]) && Bitboard::countBits(b.m_pList[wB])) 
    	return false;

    if (Bitboard::countBits(b.m_pList[bN]) && Bitboard::countBits(b.m_pList[bB])) 
    	return false;
	
    return true;
}

// Returns true of game is over, and prints reason
bool XBoard::checkResult(Board& b) 
{

    if (b.m_fiftyMove > 100) {
        std::cout << "1/2-1/2 {fifty move rule (claimed by WeeChess)}\n"; 
        return true;
    }

    if (XBoard::ThreeFoldRep(b) >= 2) {
        std::cout << "1/2-1/2 {3-fold repetition (claimed by WeeChess)}\n"; 
        return true;
    }
	
	if (XBoard::DrawMaterial(b) == true) {
        std::cout << "1/2-1/2 {insufficient material (claimed by WeeChess)}\n"; 
        return true;
    }
	
	MoveList list;
	list.genAllMoves(b);
      
	int found = 0;
	std::vector<Move>::iterator itr;
	for(itr = list.m_move_vec.begin(); itr != list.m_move_vec.end(); ++itr) {	
       
        if ( !MakeMove(b,itr->m_move)){
            continue;
        }

        found++;
		TakeMove(b);
		break;
    }
	
	if(found != 0) 
		return false;

    Colour oppostion = static_cast<Colour>(b.m_side^1);
	int InCheck = b.isSquareAttacked(b.m_kingSq[b.m_side],oppostion);
	
	if(InCheck == true)	{
	    if(b.m_side == WHITE) {
	      std::cout << "0-1 {black mates (claimed by WeeChess)}\n";
	      return true;
        } else {
	      std::cout << "0-1 {white mates (claimed by WeeChess)}\n";
	      return true;
        }
    } else {
          std::cout<<"\n1/2-1/2 {stalemate (claimed by WeeChess)}\n";
          return true;
    }	
	return false;	
}

// Print xboard settings
void XBoard::PrintOptions() 
{
	std::cout<<"feature ping=1 setboard=1 colors=0 usermove=1\n";      
	std::cout<<"feature done=1\n";
}

// Xboard/Winboard GUI protocol loo[]
void XBoard::XBoardLoop(Board& b, SearchInfo& info) 
{

	info.gameMode = XBMODE;
	info.postThinking = true;
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	PrintOptions(); // HACK
	
	int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
	int time = -1, inc = 0;                             
	int engineSide = BLACK;                    
	int timeLeft;   
	int sec;
	int mps;
	int move = 0;	
	int i, score;
	char inBuf[80], command[80];	
	
	b.parseFen(START_FEN); // Setup initial position
	
	while(true) { 

		fflush(stdout);
        // Endine searches position
		if(b.m_side == engineSide && XBoard::checkResult(b) == false) {  
			info.starttime = Search::GetTimeMs();
			info.depth = depth;
			
			if(time != -1) {
				info.timeset = true;
				time /= movestogo[b.m_side];
				time -= 50;		
				info.stoptime = info.starttime + time + inc;
			} 
	
			if(depth == -1 || depth > MAXDEPTH) {
				info.depth = MAXDEPTH;
			}
		
			std::cout<<"time:"<<time;
			std::cout<<" start:"<<info.starttime; 
			std::cout<<" stop:"<<info.stoptime; 
			std::cout<<" depth:"<<info.depth; 
			std::cout<<" timeset:"<<info.timeset; 
			std::cout<<" movestogo:"<<movestogo[b.m_side]; 
			std::cout<<" mps:"<<mps<<std::endl;

			Search::SearchPosition(b, info);
			
			if(mps != 0) {
				movestogo[b.m_side^1]--;
				if(movestogo[b.m_side^1] < 1) {
					movestogo[b.m_side^1] = mps;
				}
			}
			
		}	

		fflush(stdout); 
	
	    // Get input from gui
		memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);
		if (!fgets(inBuf, 80, stdin))
		    continue;
    
		sscanf(inBuf, "%s", command);
		
		std::cout<<"command seen:"<<inBuf<<std::endl;
    
		if(!strcmp(command, "quit")) { 
			info.quit = true;
			break; 
		}
		
		if(!strcmp(command, "force")) { 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(!strcmp(command, "protover")){
			PrintOptions();
		    continue;
		}
		
		if(!strcmp(command, "sd")) {
			sscanf(inBuf, "sd %d", &depth); 
		    std::cout << "DEBUG depth:" << depth<< std::endl;
			continue; 
		}
		
		if(!strcmp(command, "st")) {
			sscanf(inBuf, "st %d", &movetime); 
		    std::cout << "DEBUG movetime:"<<movetime<<std::endl;
			continue; 
		}  
		
		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &time); 
			time *= 10;
		    std::cout<<"DEBUG time:"<<time<<std::endl;
			continue; 
		}  

		if(!strcmp(command, "level")) {
			sec = 0;
			movetime = -1;
			if( sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
			    sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
		        std::cout << "DEBUG level with :\n";
			}else {
		         std::cout << "DEBUG level without :\n";
			}			
			timeLeft *= 60000;
			timeLeft += sec * 1000;
			movestogo[0] = movestogo[1] = 30;
			if(mps != 0) {
				movestogo[0] = movestogo[1] = mps;
			}
			time = -1;
		    std::cout<<"DEBUG level timeLeft:"<<timeLeft; 
		    std::cout<<" movesToGo:"<<movestogo[0]; 
		    std::cout<<" inc:"<<inc;
		    std::cout<<" mps:"<<mps<<std::endl;
			continue; 
		}  		
		
		if(!strcmp(command, "ping")) { 
			std::cout << "pong" << inBuf+4 << std::endl; 
			continue; 
		}
		
		if(!strcmp(command, "new")) { 
			engineSide = BLACK; 
			b.parseFen(START_FEN);
			depth = -1; 
			time = -1;
			continue; 
		}
		
		if(!strcmp(command, "setboard")){
			engineSide = BOTH;  
			b.parseFen(inBuf+9); 
			continue; 
		}   		
		
		if(!strcmp(command, "go")) { 
			engineSide = b.m_side;  
			continue; 
		}		
		  
		if(!strcmp(command, "usermove")){
			movestogo[b.m_side]--;
			move = parseMove(inBuf+9, b);	
			if(move == 0) 
				continue;
			MakeMove(b, move);
            b.m_ply=0;
		}    
    }	
}
