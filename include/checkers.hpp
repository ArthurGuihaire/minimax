#pragma once
#include <stdint.h>
#include <utility>

struct boardState {
    union {
        struct {
            uint64_t blackPawns;
            uint64_t whitePawns;
        };
        uint64_t pawns[2];
    };
    bool isWhite;
    bool isCaptureMove;
};

struct moves {
    uint64_t upLeftBitmap;
    uint64_t upRightBitmap;
    uint64_t downLeftBitmap;
    uint64_t downRightBitmap;
};

void printBitmap(uint64_t bitmap);
void printBoard(const boardState& board);
bool checkWin(const boardState& board);
moves getCaptureMoves(const boardState& board);
moves getNormalMoves(const boardState& board);
moves getMoves(boardState& board);
std::pair<boardState*, uint32_t> copyBoardWithMoves(const boardState& board, const moves& moveBitmaps);

void userChooseMove(boardState& board);
//void doMove(boardState& board, const uint64_t srcMask, const uint64_t destMask);
