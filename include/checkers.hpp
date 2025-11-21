#include <stdint.h>
//struct boardState {
//    uint64_t whitePawns;
//    uint64_t blackPawns;
//    bool isWhite;
//};

struct boardState {
    union {
        struct {
            uint64_t whitePawns;
            uint64_t blackPawns;
        };
        uint64_t pawns[2];
    };
    bool isWhite;
};

//union moves {
//    struct {
//        uint64_t upLeftBitmap, upRightBitmap, downLeftBitmap, downRightBitmap;
//    };
//    uint64_t bitmaps[4];
//}

struct moves {
    uint64_t upLeftBitmap;
    uint64_t upRightBitmap;
    uint64_t downLeftBitmap;
    uint64_t downRightBitmap;
};

void printBoard(const boardState& board);
moves getCaptureMoves(const boardState& board);
moves getNormalMoves(const boardState& board);
moves getMoves(const boardState& board);

void doMove(const uint64_t srcMask, const uint64_t destMask, boardState& board);
