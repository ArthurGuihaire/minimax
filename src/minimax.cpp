#include <minimax.hpp>
#include <checkers.hpp>
#include <constants.hpp>

int32_t getScore(const boardState& board) {
    const uint32_t blackScore = __builtin_popcountll(board.blackPawns);
    const uint32_t whiteScore = __builtin_popcountll(board.whitePawns);
    //return (whiteScore - blackScore) * ((board.isWhite << 1) - 1);
    return (board.isWhite) ? (whiteScore - blackScore) : (blackScore - whiteScore);
}

int32_t minimax(boardState& board, uint32_t currentDepth) {
    moves possibleMoves = getMoves(board); //this also updates isWhite

    if (checkWin(board)) return board.isWhite ? INT32_MAX : INT32_MIN; //checkwin after isWhite is correct

    std::pair<boardState*, uint32_t> boardStateArray = copyBoardWithMoves(board, possibleMoves);
    
    if (++currentDepth >= maxSearchDepth) {
        uint32_t maxScore = getScore(boardStateArray.first[0]);
        for (uint32_t i = 1; i < boardStateArray.second; i++) {
            uint32_t score = getScore(boardStateArray.first[i]);
            if (score > maxScore) maxScore = score;
        }
        return maxScore;
    }
    else {
        uint32_t maxScore = minimax(board, currentDepth + 1);
        for (uint32_t i = 1; i < boardStateArray.second; i++) {
            uint32_t score = minimax(boardStateArray.first[i], currentDepth + 1);
            if (score > maxScore) maxScore = score;
        }
        return maxScore;
    }
}

//runs one iteration of minimax
void runMinimax(boardState &board) {
    moves possibleMoves = getMoves(board);
    
    const uint32_t scoreReinvert = board.isWhite ? 1 : -1;
    
    std::pair<boardState*, uint32_t> boardStateArray = copyBoardWithMoves(board, possibleMoves);

    uint32_t bestMove = 0;
    uint32_t bestScore = INT32_MIN;
    for (uint32_t i = 0; i < boardStateArray.second; i++) {
        uint32_t score = minimax(boardStateArray.first[i], 1);
        if (scoreReinvert * score > bestScore) {
            bestScore = score;
            bestMove = i;
        }
    }
    
    board = boardStateArray.first[bestMove];
}
