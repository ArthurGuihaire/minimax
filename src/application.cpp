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
    if (isWhite) std::cout << "Black to move" << std::endl;
    else std::cout << "White to move" << std::endl;
}

void Application::runGame() {
    bool running = true;
    printBoard(board);
    printPlayerTurn(board.isWhite);
    while (running) {
        //first player move
        if (numPlayers == 0) {
            runMinimax(board);
        }
        else {
            userChooseMove(board);
        }
        printBoard(board);
        printPlayerTurn(board.isWhite);
        
        //second player move
        if (numPlayers == 2) {
            userChooseMove(board);
        }
        else {
            runMinimax(board);
        }
        printBoard(board);
        printPlayerTurn(board.isWhite);
    }
}
