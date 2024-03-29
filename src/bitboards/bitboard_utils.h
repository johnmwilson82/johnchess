#pragma once
#include <cstdint>
#include <cassert>

namespace bitboard_utils
{
    static constexpr uint8_t debruijn_index64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };
    
    static constexpr uint64_t debruijn64 = 0x03f79d71b4cb0a89;

    // from https://www.chessprogramming.org/BitScan
    static constexpr inline uint8_t bit_scan_forward(uint64_t bb) {
        assert(bb != 0);

        return debruijn_index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
    }

    static constexpr inline uint8_t bit_scan_reverse(uint64_t bb) {
        assert(bb != 0);

        bb |= bb >> 1;
        bb |= bb >> 2;
        bb |= bb >> 4;
        bb |= bb >> 8;
        bb |= bb >> 16;
        bb |= bb >> 32;

        return debruijn_index64[(bb * debruijn64) >> 58];
    }

    static inline uint64_t mirror_vertical(uint64_t bb) {
        return _byteswap_uint64(bb);
    }

    static inline uint64_t pop_count(uint64_t bb) {
        return __popcnt64(bb);
    }
}