#pragma once
#include <stdint.h>
constexpr inline uint64_t startingPositionWhite = 0x0000000000AA55AA;
constexpr inline uint64_t startingPositionBlack = 0x55AA550000000000;
constexpr inline uint32_t maxSearchDepth = 8;

constexpr inline uint32_t gpuBufferMultiple = 256;

extern uint32_t windowWidth, windowHeight;
