#pragma once
#include <stdint.h>
constexpr inline uint64_t startingPositionWhite = 0x0000000000AA55AA;
constexpr inline uint64_t startingPositionBlack = 0x55AA550000000000;
constexpr inline uint32_t maxSearchDepth = 8;

constexpr static uint64_t zero[4] = {0, 0, 0, 0};

extern uint32_t windowWidth, windowHeight;

constexpr inline float square[16] = {
    -0.05, -0.05, 0, 0,
    0.05, -0.05, 1, 0,
    -0.05, 0.05, 0, 1,
    0.05, 0.05, 1, 1
};
