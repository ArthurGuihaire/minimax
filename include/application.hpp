#pragma once
#include <checkers.hpp>
class Application {
    public:
        Application(const uint32_t numPlayers);
        void runGame();
    private:
        boardState board;
        uint32_t numPlayers;
};