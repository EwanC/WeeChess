#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include "board.hpp"
#include "perft.hpp"
#include "protocols.hpp"
#include "search.hpp"

void printWelcome(Mode mode) {
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
  } else if (mode == UCIMODE) {
    std::cout << "\nWeeChess is running in UCI mode\n\n";
  }
}

Mode parseCommandLineArgs(int argc, char* argv[]) {
  Mode mode = UCIMODE;  // default mode

  for (int i = 1; i < argc; ++i) {
    // print help and exit
    if (strncmp(argv[i], "--help", 6) == 0 || strncmp(argv[i], "-h", 2) == 0) {
      CLI::printUsage();
      exit(0);
    }

    // UCI
    if (strncmp(argv[i], "--uci", 5) == 0 || strncmp(argv[i], "-u", 2) == 0) {
      mode = UCIMODE;
      break;
    }

    // XBoard
    if (strncmp(argv[i], "--xboard", 8) == 0 || strncmp(argv[i], "-x", 2) == 0) {
      mode = XBMODE;
      break;
    }

    // Command line
    if (strncmp(argv[i], "--cli", 5) == 0 || strncmp(argv[i], "-c", 2) == 0) {
      mode = CONSOLEMODE;
      break;
    }

    // Run Perft tests
    if (strncmp(argv[i], "--perf", 6) == 0 || strncmp(argv[i], "-p", 2) == 0) {
      mode = PERFTMODE;
      break;
    }
  }

  return mode;
}

int main(int argc, char* argv[]) {
  const Mode protocol = parseCommandLineArgs(argc, argv);

  printWelcome(protocol);

  Board::initStaticMembers();
  Board b;

  if (protocol == PERFTMODE) {
    const int level = 3;
    std::cout << "Running Perft to level " << level << std::endl;
    if (runPerft(b, level)) {
      std::cout << "Perft passed\n";
    } else {
      std::cout << "Perft failed\n";
    }
  } else {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    SearchInfo info;
    std::function<void(Board&, SearchInfo&)> gameLoop = nullptr;
    // Pick protocol to interact with engine
    if (protocol == UCIMODE) {
      gameLoop = &UCI::UCILoop;
    } else if (protocol == XBMODE) {
      gameLoop = &XBoard::XBoardLoop;
    } else if (protocol == CONSOLEMODE) {
      gameLoop = &CLI::consoleLoop;
    } else {
      assert("Unknown protocol\n");
    }

    while (false == info.quit) {
      fflush(stdout);
      gameLoop(b, info);
    }
  }
  return 0;
}
