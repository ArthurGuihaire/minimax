#include "checkers.hpp"
#include <gpuBuffer.hpp>

enum selectionState {
    waiting, validMoves, selected
};

class Renderer {
    public:
        Renderer(boardState& initialBoard);
        void updatePositions();
        void render();
    private:
        boardState& board;
        uint32_t numPieces;

        gpuBuffer vertexBuffer;
        gpuBuffer positionsBuffer;

        uint32_t vao;
        uint32_t textureShader;
        uint32_t textureUniformLocation;
};
