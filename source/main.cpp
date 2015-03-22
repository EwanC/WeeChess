#include <iostream>
#include "protocols.hpp"
#include <cstring>

void printWelcome(Mode mode)
{
    std::cout << std::endl;
    std::cout << "| |  | |    | |                          | |        | |  | |         /  __ \\ |  "
                 "                \n";
    std::cout << "| |  | | ___| | ___ ___  _ __ ___   ___  | |_ ___   | |  | | ___  ___| /  \\/ "
                 "|__   ___  ___ ___ \n";
    std::cout << "| |/\\| |/ _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  | |/\\| |/ _ \\/ _ \\ "
                 "|   | '_ \\ / _ \\/ __/ __|\n";
    std::cout << "\\  /\\  /  __/ | (_| (_) | | | | | |  __/ | || (_) | \\  /\\  /  __/  __/ "
                 "\\__/\\ | | |  __/\\__ \\__ \\\n";
    std::cout << " \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/   \\/  \\/ "
                 "\\___|\\___|\\____/_| |_|\\___||___/___/\n\n";

    if (mode == XBMODE) {
        std::cout << "\nWeeChess is running in XBoard/WinBoard mode\n\n";
    }
    else if (mode == UCIMODE) {
        std::cout << "\nWeeChess is running in UCI mode\n\n";
    }
}

Mode parseCommandLineArgs(int argc, char* argv[])
{
    Mode mode = UCIMODE; // default mode

    for (int i = 1; i < argc; ++i) {

        if (strncmp(argv[i], "--help", 6) == 0 || strncmp(argv[i], "-h", 2) == 0) // print help
        {
            CLI::printUsage();
            exit(0);
        }

        if (strncmp(argv[i], "--uci", 5) == 0 || strncmp(argv[i], "-u", 2) == 0) // UCI
        {
            mode = UCIMODE;
            break;
        }

        if (strncmp(argv[i], "--xboard", 8) == 0 || strncmp(argv[i], "-x", 2) == 0) // XBoard
        {
            mode = XBMODE;
            break;
        }

        if (strncmp(argv[i], "--cli", 5) == 0 || strncmp(argv[i], "-c", 2) == 0) // Command line
        {
            mode = CONSOLEMODE;
            break;
        }
    }

    return mode;
}

int main(int argc, char* argv[])
{

    Mode protocol = parseCommandLineArgs(argc, argv);

    Board b;
    Board::initStaticMembers(); // TIDY UP
    Search::InitEvalMasks();    // TIDY UP

    SearchInfo info;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    printWelcome(protocol);

    while (true) {

        fflush(stdout);

        // Pick protocl to interact with engine, TODO parse these from invoke args
        if (protocol == UCIMODE) {
            UCI::UCILoop(b, info);

            if (info.quit == true)
                break;
            continue;
        }
        else if (protocol == XBMODE) {
            XBoard::XBoardLoop(b, info);

            if (info.quit == true)
                break;
            continue;
        }
        else if (protocol == CONSOLEMODE) {
            CLI::consoleLoop(b, info);

            if (info.quit == true)
                break;
            continue;
        }
    }

    return 0;
}
