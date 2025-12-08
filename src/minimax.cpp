#include <minimax.hpp>
#include <checkers.hpp>
#include <constants.hpp>

int32_t getScore(const boardState& board) {
    const uint32_t blackScore = __builtin_popcountll(board.blackPawns);
    const uint32_t whiteScore = __builtin_popcountll(board.whitePawns);
    //return (whiteScore - blackScore) * ((board.isWhite << 1) - 1);
    return whiteScore - blackScore;
}

int32_t minimax(boardState board, uint32_t currentDepth) { //pass by value, we want a copy
    const moves possibleMoves = getMoves(board); //updates isCaptureMove and isWhite
    if (checkWin(board)) return board.isWhite ? INT32_MAX : INT32_MIN; //checkwin after isWhite is correct

    boardState boardStateArray[48];
    const uint32_t numBoards = copyBoardWithMoves(board, possibleMoves, boardStateArray);
    if (board.isWhite) {
        int32_t maxScore;
        if (currentDepth >= maxSearchDepth) {
            maxScore = getScore(boardStateArray[0]);
            for (uint32_t i = 0; i < numBoards; i++) {
                uint32_t score = getScore(boardStateArray[i]);
                if (score > maxScore) maxScore = score;
            }
        }
        else {
            maxScore = minimax(boardStateArray[0], currentDepth + 1);
            for (uint32_t i = 1; i < numBoards; i++) {
                int32_t score = minimax(boardStateArray[i], currentDepth + 1);
                if (score > maxScore) maxScore = score;
            }
        }
        return maxScore;
    }
    else {
        int32_t minScore;
        if (currentDepth >= maxSearchDepth) {
            minScore = getScore(boardStateArray[0]);
            for (uint32_t i = 0; i < numBoards; i++) {
                uint32_t score = getScore(boardStateArray[i]);
                if (score < minScore) minScore = score;
            }
        }
        else {
            minScore = minimax(boardStateArray[0], currentDepth + 1);
            for (uint32_t i = 1; i < numBoards; i++) {
                int32_t score = minimax(boardStateArray[i], currentDepth + 1);
                if (score > minScore) minScore = score;
            }
        }
        return minScore;
    }
    
}

//runs one iteration of minimax
void runMinimax(boardState &board) {
    moves possibleMoves = getMoves(board);
    boardState boardStateArray[48];
    
    const uint32_t numBoards = copyBoardWithMoves(board, possibleMoves, boardStateArray);

    uint32_t bestMove = 0;
    if (board.isWhite) {
        int32_t maxScore = minimax(boardStateArray[0], 1);
        for (uint32_t i = 1; i < numBoards; i++) {
            int32_t score = minimax(boardStateArray[i], 1);
            if (score > maxScore) {
                score = maxScore;
                bestMove = i;
            }
        }
    }
    else {
        int32_t minScore;
        minScore = minimax(boardStateArray[0], 1);
        for (uint32_t i = 1; i < numBoards; i++) {
            int32_t score = minimax(boardStateArray[i], 1);
            if (score < minScore) {
                score = minScore;
                bestMove = i;
            }
        }
    }
    
    board = boardStateArray[bestMove];
}
