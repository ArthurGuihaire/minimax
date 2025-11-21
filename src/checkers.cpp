#include <checkers.hpp>
#include <iostream>
#include <cstring> // memcpy and memcmp

void printBitmap(uint64_t bitmap) {
    uint64_t extractBit = 0x8000000000000000;
    for (uint32_t i = 0; i < 64; i++) {
        uint64_t bit = bitmap & extractBit;
        if (bit)
            std::cout << 'X';
        else
            std::cout << '-';
        extractBit >>= 1;
        if (i%8 == 7)
            std::cout << '\n';
    }
    std::cout << std::endl;
}

void printBoard(const boardState& board) {
    for (int32_t i = 63; i >= 0; i--) {
        char blackBit = board.blackPawns >> i & 0x1;
        char whiteBit = board.whitePawns >> i & 0x1;
        if (blackBit && whiteBit)
            std::cout << "Error: two pawns in slot " << i << std::endl;
        else {
            if (blackBit)
                std::cout << "O";
            else if (whiteBit)
                std::cout << "X";
            else
                std::cout << "-";
        }
        if (i % 8 == 0)
            std::cout << "\n";
    }
    std::cout << std::endl;
}

//Bitmaps for board manipulation
constexpr static uint64_t maskLeft2 = 0x3F3F3F3F3F3F3F3F;
constexpr static uint64_t maskLeft1 = 0x7F7F7F7F7F7F7F7F;
constexpr static uint64_t maskRight2 = 0xFCFCFCFCFCFCFCFC;
constexpr static uint64_t maskRight1 = 0xFEFEFEFEFEFEFEFE;

moves getCaptureMoves(const boardState& board) {
    const uint64_t emptySquares = ~(board.blackPawns | board.whitePawns);
    //upleft capture move
    uint64_t emptyShifted = emptySquares >> 18 & maskLeft2; // mask left 2 columns
    uint64_t opponentShifted = board.pawns[!board.isWhite] >> 9 & maskLeft1; // mask left columns
    uint64_t upLeftCaptures = board.pawns[board.isWhite] & opponentShifted & emptyShifted;

    //up right
    emptyShifted = emptySquares >> 14 & maskRight2; // mask right two columns
    opponentShifted = board.pawns[!board.isWhite] >> 7 & maskRight1; // mask right column
    uint64_t upRightCaptures = board.pawns[board.isWhite] & opponentShifted & emptyShifted;
    
    //down left
    emptyShifted = emptySquares << 14 & maskLeft2;
    opponentShifted = board.pawns[!board.isWhite] << 7 & maskLeft1;
    uint64_t downLeftCaptures = board.pawns[board.isWhite] & opponentShifted & emptyShifted;

    //down right
    emptyShifted = emptySquares << 18 & maskRight2;
    opponentShifted = board.pawns[!board.isWhite] << 9 & maskRight1;
    uint64_t downRightCaptures = board.pawns[board.isWhite] & opponentShifted & emptyShifted;

    return {upLeftCaptures, upRightCaptures, downLeftCaptures, downRightCaptures};
}

moves getNormalMoves(const boardState& board) {
    const uint64_t emptySquares = ~(board.blackPawns | board.whitePawns);
    if (board.isWhite) {
        //move left
        uint64_t emptyShifted = emptySquares >> 9 & maskLeft1;
        uint64_t leftMoves = emptyShifted & board.whitePawns;

        //move right
        emptyShifted = emptySquares >> 7 & maskRight1;
        uint64_t rightMoves = emptyShifted & board.whitePawns;

        return {leftMoves, rightMoves, 0, 0};
    }
    else {
        //move left
        uint64_t emptyShifted = emptySquares << 7 & maskLeft1;
        uint64_t leftMoves = emptyShifted & board.blackPawns;

        //move right
        emptyShifted = emptySquares << 9 & maskRight1;
        uint64_t rightMoves = emptyShifted & board.blackPawns;

        return {0, 0, leftMoves, rightMoves};
    }
}

moves getMoves(const boardState& board) {
    constexpr uint64_t zero[4] = {0, 0, 0, 0};
    moves possibleMoves = getCaptureMoves(board);
    if (!std::memcmp(&possibleMoves, zero, sizeof(moves))) // memcmp returns 0 if they're the same, so invert
        possibleMoves = getNormalMoves(board);
    return possibleMoves;
}

void copyBoardWithMoves(const boardState& board, const moves& moveBitmaps) {
    constexpr static uint64_t one64Bits = 0x1; // 0x1 is 32 bits by default, we need 64 bits
    static boardState boardCopies[48];
    
    printBitmap(moveBitmaps.downLeftBitmap);
    printBitmap(moveBitmaps.downRightBitmap);
    printBitmap(moveBitmaps.upLeftBitmap);
    printBitmap(moveBitmaps.upRightBitmap);

    uint32_t copyIndex;
    uint32_t numMoves = __builtin_popcountll(moveBitmaps.upLeftBitmap);
    uint64_t remainingMoves = moveBitmaps.upLeftBitmap;
    for (copyIndex = 0; copyIndex < numMoves; copyIndex++) {
        uint32_t index = __builtin_ctzl(remainingMoves);
        uint64_t srcBitmap = ~(one64Bits << index);
        uint64_t destBitmap = one64Bits << (index + 9);
        std::memcpy(&(boardCopies[copyIndex]), &board, sizeof(moves)); // Possible optimization: Copy only 17 bytes not 24
        boardCopies[copyIndex].pawns[board.isWhite] &= srcBitmap;
        boardCopies[copyIndex].pawns[board.isWhite] |= destBitmap;
        //delete the move from available moves
        remainingMoves &= srcBitmap;    
    }
    numMoves = __builtin_popcountll(moveBitmaps.upRightBitmap);
    uint32_t stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.upRightBitmap;
    for (; copyIndex < stopIndex; copyIndex++) {
        uint32_t index = __builtin_ctzl(remainingMoves);
        uint64_t srcBitmap = ~(one64Bits << index);
        uint64_t destBitmap = one64Bits << (index + 7);
        std::memcpy(&(boardCopies[copyIndex]), &board, sizeof(moves)); // Possible optimization: Copy only 17 bytes not 24
        boardCopies[copyIndex].pawns[board.isWhite] &= srcBitmap;
        boardCopies[copyIndex].pawns[board.isWhite] |= destBitmap;
        //delete the move from available moves
        remainingMoves &= srcBitmap;    
    }
    numMoves = __builtin_popcountll(moveBitmaps.downLeftBitmap);
    stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.downLeftBitmap;
    for (; copyIndex < stopIndex; copyIndex++) {
        uint32_t index = __builtin_ctzl(remainingMoves);
        uint64_t srcBitmap = ~(one64Bits << index);
        uint64_t destBitmap = one64Bits << (index - 7);
        std::memcpy(&(boardCopies[copyIndex]), &board, sizeof(moves)); // Possible optimization: Copy only 17 bytes not 24
        boardCopies[copyIndex].pawns[board.isWhite] &= srcBitmap;
        boardCopies[copyIndex].pawns[board.isWhite] |= destBitmap;
        //delete the move from available moves
        remainingMoves &= srcBitmap;    
    }
    numMoves = __builtin_popcountll(moveBitmaps.downRightBitmap);
    stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.downRightBitmap;
    for (; copyIndex < stopIndex; copyIndex++) {
        uint32_t index = __builtin_ctzl(remainingMoves);
        uint64_t srcBitmap = ~(one64Bits << index);
        uint64_t destBitmap = one64Bits << (index - 9);
        std::memcpy(&(boardCopies[copyIndex]), &board, sizeof(moves)); // Possible optimization: Copy only 17 bytes not 24
        boardCopies[copyIndex].pawns[board.isWhite] &= srcBitmap;
        boardCopies[copyIndex].pawns[board.isWhite] |= destBitmap;
        //delete the move from available moves
        remainingMoves &= srcBitmap;    
    }
    
    for (uint32_t i = 0; i < copyIndex; i++) { 
        printBoard(boardCopies[i]);
    }
}

void doMove(const uint64_t srcMask, const uint64_t destMask, boardState& board) {
    if (board.isWhite) {
        board.whitePawns &= (~srcMask);
        board.whitePawns |= destMask;
    }
    else {
        board.blackPawns &= (~srcMask);
        board.blackPawns |= destMask;
    }
}
