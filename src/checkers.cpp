#include <checkers.hpp>
#include <iostream>
#include <cstring> // memcpy and memcmp
#include <constants.hpp>

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

//Must be called BEFORE making a move
bool checkWin(const boardState& board) {
    const uint64_t lastPlayerPawns = board.pawns[!board.isWhite];
    return __builtin_popcountll(lastPlayerPawns) == 0;
}

//magic values
//Bitmaps for board manipulation
constexpr static uint64_t maskLeft2 = 0x3F3F3F3F3F3F3F3F;
constexpr static uint64_t maskLeft1 = 0x7F7F7F7F7F7F7F7F;
constexpr static uint64_t maskRight2 = 0xFCFCFCFCFCFCFCFC;
constexpr static uint64_t maskRight1 = 0xFEFEFEFEFEFEFEFE;
// 0x1 is 32 bits by default, we need 64 bits
constexpr inline static uint64_t one64Bits = 0x1; 

static moves getCaptureMoves(const boardState& board) {
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

static moves getNormalMoves(const boardState& board) {
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

moves getSpecificCaptureMove(const boardState& board) {
    const uint64_t emptySquares = ~(board.blackPawns | board.whitePawns);
    const uint64_t currentSquare = one64Bits << board.srcPosition;
    //upleft capture move
    uint64_t emptyShifted = emptySquares >> 18 & maskLeft2; // mask left 2 columns
    uint64_t opponentShifted = board.pawns[!board.isWhite] >> 9 & maskLeft1; // mask left columns
    const uint64_t upLeftCaptures = currentSquare & opponentShifted & emptyShifted;

    //up right
    emptyShifted = emptySquares >> 14 & maskRight2; // mask right two columns
    opponentShifted = board.pawns[!board.isWhite] >> 7 & maskRight1; // mask right column
    const uint64_t upRightCaptures = currentSquare & opponentShifted & emptyShifted;
    
    //down left
    emptyShifted = emptySquares << 14 & maskLeft2;
    opponentShifted = board.pawns[!board.isWhite] << 7 & maskLeft1;
    const uint64_t downLeftCaptures = currentSquare & opponentShifted & emptyShifted;

    //down right
    emptyShifted = emptySquares << 18 & maskRight2;
    opponentShifted = board.pawns[!board.isWhite] << 9 & maskRight1;
    const uint64_t downRightCaptures = currentSquare & opponentShifted & emptyShifted;

    return {upLeftCaptures, upRightCaptures, downLeftCaptures, downRightCaptures};
}

moves getMoves(boardState& board) {
    //constexpr uint64_t zero[4] = {0, 0, 0, 0};
    //moves possibleMoves = getCaptureMoves(board);
    //if (!std::memcmp(&possibleMoves, zero, sizeof(moves))) { // memcmp returns 0 if they're the same, so invert
    //    if (board.isCaptureMove) {
    //        board.isCaptureMove = false;
    //        board.isWhite = !board.isWhite;
    //        return getMoves(board);
    //    }
    //    possibleMoves = getNormalMoves(board);
    //}
    //else {
    //    board.isCaptureMove = true;
    //}
    //return possibleMoves;
    if (!board.isCaptureMove) board.isWhite = !board.isWhite;
    moves possibleMoves;
    if (board.isCaptureMove) {
        possibleMoves = getSpecificCaptureMove(board);
        if (memcmp(&possibleMoves, zero, sizeof(moves))) return possibleMoves; // if its not zero there's valid moves
        else {
            //can't move that pawn, so find a move as other player
            board.isCaptureMove = false;
            board.isWhite = !board.isWhite;
        }
    }
    possibleMoves = getCaptureMoves(board);
    if (memcmp(&possibleMoves, zero, sizeof(moves))) {
        board.isCaptureMove = true;
    }
    else {
        board.isCaptureMove = false;
        possibleMoves = getNormalMoves(board);
    }
    return possibleMoves;
}

//used in copyBoardWithMoves and userChooseMove

inline uint64_t doMoveUp(boardState& board, uint32_t rawBitOffset, bool isCaptureMove) {
    uint64_t srcMask = one64Bits << board.srcPosition;
    const uint64_t destMask = srcMask << (rawBitOffset << isCaptureMove); //double raw bit offset if capture, since capturing moves 2 squares
    srcMask |= ((uint64_t)isCaptureMove) << (board.srcPosition + rawBitOffset); // adds a 1 to remove opponent pawn if its a capture move
    board.whitePawns &= (~srcMask);
    board.blackPawns &= (~srcMask);
    board.pawns[board.isWhite] |= destMask; //apply the mask for the new moved pawn position
    return srcMask;
}

inline uint64_t doMoveDown(boardState& board, uint32_t rawBitOffset, bool isCaptureMove) {
    uint64_t srcMask = one64Bits << board.srcPosition;
    const uint64_t destMask = srcMask >> (rawBitOffset << isCaptureMove); //double raw bit offset if capture, since capturing moves 2 squares
    srcMask |= ((uint64_t)isCaptureMove) << (board.srcPosition - rawBitOffset); // adds a 1 to remove opponent pawn if its a capture move
    //board.pawns[board.isWhite] &= (~srcMask); // invert the src mask, to cancel out bits
    board.whitePawns &= (~srcMask);
    board.blackPawns &= (~srcMask);
    board.pawns[board.isWhite] |= destMask; //apply the mask for the new moved pawn position
    return srcMask;
}

//returns number of moves
uint32_t copyBoardWithMoves(const boardState& board, const moves& moveBitmaps, boardState* writeBoardArray) {
    uint32_t copyIndex;
    uint32_t numMoves = __builtin_popcountll(moveBitmaps.upLeftBitmap);
    uint64_t remainingMoves = moveBitmaps.upLeftBitmap;
    //up left move: bit offset left 9
    for (copyIndex = 0; copyIndex < numMoves; copyIndex++) {
        std::memcpy(&(writeBoardArray[copyIndex]), &board, sizeof(boardState));
        writeBoardArray[copyIndex].srcPosition = __builtin_ctzl(remainingMoves);

        uint64_t srcBitmap = doMoveUp(writeBoardArray[copyIndex], 9, board.isCaptureMove);
        //delete the move from available moves
        remainingMoves &= srcBitmap;
    }
    numMoves = __builtin_popcountll(moveBitmaps.upRightBitmap);
    uint32_t stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.upRightBitmap;
    //up right move: bit offset left 7
    for (; copyIndex < stopIndex; copyIndex++) {
        std::memcpy(&(writeBoardArray[copyIndex]), &board, sizeof(boardState));
        writeBoardArray[copyIndex].srcPosition = __builtin_ctzl(remainingMoves);

        uint64_t srcBitmap = doMoveUp(writeBoardArray[copyIndex], 7, board.isCaptureMove);

        remainingMoves &= srcBitmap;
    }
    numMoves = __builtin_popcountll(moveBitmaps.downLeftBitmap);
    stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.downLeftBitmap;
    //down left move: bit offset right 7
    for (; copyIndex < stopIndex; copyIndex++) {
        std::memcpy(&(writeBoardArray[copyIndex]), &board, sizeof(boardState));
        writeBoardArray[copyIndex].srcPosition = __builtin_ctzl(remainingMoves);

        uint64_t srcBitmap = doMoveDown(writeBoardArray[copyIndex], 7, board.isCaptureMove);

        remainingMoves &= srcBitmap;
    }
    numMoves = __builtin_popcountll(moveBitmaps.downRightBitmap);
    stopIndex = numMoves + copyIndex;
    remainingMoves = moveBitmaps.downRightBitmap;
    for (; copyIndex < stopIndex; copyIndex++) {
        std::memcpy(&(writeBoardArray[copyIndex]), &board, sizeof(boardState));

        writeBoardArray[copyIndex].srcPosition = __builtin_ctzl(remainingMoves);
        uint64_t srcBitmap = doMoveDown(writeBoardArray[copyIndex], 9, board.isCaptureMove);

        remainingMoves &= srcBitmap;    
    }
    
    return stopIndex;
}

void userChooseMove(boardState& board) {
    const moves validMoves = getMoves(board);
    int32_t xSrc, ySrc, xDest, yDest;
choose_coords_label:
    do {
        std::cout << "x coord src: " << std::flush;
        std::cin >> xSrc;

        std::cout << "y coord src: " << std::flush;
        std::cin >> ySrc;
    } while ((board.pawns[board.isWhite] >> (63 - 8 * ySrc - xSrc) & one64Bits) != 1 || !(0 <= xSrc && xSrc < 8 && 0 <= ySrc && ySrc < 8));

    do {
        std::cout << "x coord dest: " << std::flush;
        std::cin >> xDest;

        std::cout << "y coord dest: " << std::flush;
        std::cin >> yDest;
    } while ((((board.blackPawns | board.whitePawns) >> (63 - 8 * yDest - xDest)) & one64Bits) != 0 || !(0 <= xDest && xDest < 8 && 0 <= yDest && yDest < 8));

    const uint32_t moveDistance = board.isCaptureMove ? 2 : 1;
    if (std::abs(xSrc - xDest) != moveDistance) goto choose_coords_label;
    if (std::abs(ySrc - yDest) != moveDistance) goto choose_coords_label;
    
    board.srcPosition = 63 - 8 * ySrc - xSrc; // srcBitmap is inverted in doMove

    std::cout << board.isCaptureMove << "\n";
    if (xDest < xSrc && yDest < ySrc) {
        if ((validMoves.upLeftBitmap >> (63 - 8 * ySrc - xSrc) & one64Bits) == 0)
            goto choose_coords_label;
        else
            doMoveUp(board, 9, board.isCaptureMove);
    }
    else if (xDest > xSrc && yDest < ySrc) {
        if ((validMoves.upRightBitmap >> (63 - 8 * ySrc - xSrc) & one64Bits) == 0)
            goto choose_coords_label;
        else
            doMoveUp(board, 7, board.isCaptureMove);

    }
    else if (xDest < xSrc && yDest > ySrc) {
        if ((validMoves.downLeftBitmap >> (63 - 8 * ySrc - xSrc) & one64Bits) == 0)
            goto choose_coords_label;
        else
            doMoveDown(board, 7, board.isCaptureMove);

    }
    else if (xDest > xSrc && yDest > ySrc) {
        if ((validMoves.downRightBitmap >> (63 - 8 * ySrc - xSrc) & one64Bits) == 0)
            goto choose_coords_label;
        else
            doMoveDown(board, 9, board.isCaptureMove);
    }
    else {
        std::cout << "Something went wrong" << std::endl;
    }
}
