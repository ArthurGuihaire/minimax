#include <GL/glew.h>
#include <renderer.hpp>
#include <shaderLoader.hpp>
#include <constants.hpp>
#include <cstring>

Renderer::Renderer(boardState& initialBoard)
 : board(initialBoard) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vertexBuffer = gpuBuffer(square, sizeof(square), GL_ARRAY_BUFFER);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    positionsBuffer = gpuBuffer(nullptr, 512, GL_ARRAY_BUFFER);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    
    textureShader = createShader("shaders/texture.vert", "shaders/texture.frag");
    textureUniformLocation = glGetUniformLocation(textureShader, "inputTexture");
    glUseProgram(textureShader);
}

void Renderer::updatePositions() {
    float positionsArray[48];
    uint64_t pawns[2];
    std::memcpy(pawns, board.pawns, sizeof(pawns));
    numPieces = 0;
    
    positionsBuffer.clear();
    
    for (uint32_t i = 0; i < 2; i++) {
        while (std::memcmp(&pawns[i], zero, sizeof(pawns[i]))) {
            uint32_t bitOffset = __builtin_ctzl(pawns[i]);
            float coords[2] = {(bitOffset / 8) / 8.0f, (bitOffset % 8) / 8.0f};
            positionsBuffer.addData(coords, sizeof(coords));
            ++numPieces;
        }
    }
}

void Renderer::render() {
    glUniform1i(textureUniformLocation, 0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numPieces);
}
