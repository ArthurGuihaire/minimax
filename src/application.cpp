#include <application.hpp>
#include <minimax.hpp>
#include <constants.hpp>
#include <cstdlib> // for random

Application::Application(const uint32_t numPlayers) :
    numPlayers(numPlayers)
{
    bool startAsWhite = std::rand() & 1;
    board = {{{startingPositionBlack, startingPositionWhite}}, startAsWhite, false};
}

void Application::runGame() {
    bool running = true;
    printBoard(board);
    while (running) {
        //first player move
        if (numPlayers == 0) {
            runMinimax(board);
        }
        else {
            userChooseMove(board);
        }
        printBoard(board);
        
        //second player move
        if (numPlayers == 2) {
            userChooseMove(board);
        }
        else {
            runMinimax(board);
        }
        printBoard(board);
    }
}
