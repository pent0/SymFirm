#pragma once

#include <cstdint>

namespace utils {
    template <typename T>
    constexpr T min(T a, T b) {
        return a > b ? b : a;
    }

    template <typename T>
    constexpr T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    constexpr T mbswap(T num);

    constexpr uint16_t mbswap(uint16_t num) {
        uint32_t temp = ((num & 0x00FF) << 8) | ((num & 0xFF00) >> 8);
        return (uint16_t)(temp);
    }

    constexpr uint32_t mbswap(uint32_t num) {
        return ((num & 0x000000FF) << 24) |
                ((num & 0x0000FF00) <<  8) |
                ((num & 0x00FF0000) >>  8) |
                ((num & 0xFF000000) >> 24);
    }
}