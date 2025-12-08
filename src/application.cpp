#include <application.hpp>
#include <minimax.hpp>
#include <constants.hpp>
#include <cstdlib> // for random
#include <iostream>

Application::Application(const uint32_t numPlayers) :
    numPlayers(numPlayers)
{
    bool startAsWhite = std::rand() & 1;
    board = {{{startingPositionBlack, startingPositionWhite}}, startAsWhite, false};
}

void printPlayerTurn(bool isWhite) {
    if (isWhite) std::cout << "White to move" << std::endl;
    else std::cout << "Black to move" << std::endl;
}

void Application::runGame() {
    bool running = true;
    printBoard(board);
    while (running) {
        printPlayerTurn(board.isWhite);
        if (numPlayers == 0 || (numPlayers == 1 && board.isWhite)) {
            runMinimax(board);
            std::cin.get();
        }
        else {
            userChooseMove(board);
        }
        printBoard(board);
    }
}
