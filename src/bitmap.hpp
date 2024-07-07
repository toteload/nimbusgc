#pragma once

#include "common.hpp"
#include <vector>

constexpr u32 BIT_NOT_FOUND = UINT32_MAX;
constexpr u64 ALL_ONES_64   = UINT64_MAX;

inline void set_bits(u64 *bits, u32 start, u32 count) {
    u32 end = start + count;

    u32 idx_start = start / 64;
    u32 idx_end   = end   / 64;

    u64 mask_start = ALL_ONES_64 << (start % 64);
    u64 mask_end   = ALL_ONES_64 >> (63 - (end % 64));

    if (idx_start == idx_end) {
        bits[idx_start] |= mask_start & (mask_end >> 1);
    } else {
        bits[idx_start] |= mask_start;
        bits[idx_end]   |= mask_end;
    }

    for (u32 i = idx_start + 1; i < idx_end; i++) {
        bits[i] = ALL_ONES_64;
    }
}

inline u32 find_first_zero_bit(u64 *bits, u64 *end, u32 start) {
    u32 idx    = start / 64;
    u32 offset = start % 64;

    u64 *p = bits + idx;
    u64 mask = (offset == 0) ? 0 : (ALL_ONES_64 >> (64 - offset));
    u64  x = *p | mask;
    if (x != ALL_ONES_64) {
        return idx * 64 + count_trailing_ones(x);
    }

    p++;

    while (*p == ALL_ONES_64 && p != end) {
        p++;
    }

    if (p == end) {
        return BIT_NOT_FOUND;
    }

    return (p - bits) * 64 + count_trailing_ones(*p);
}

inline u32 find_first_one_bit(u64 *bits, u64 *end, u32 start) {
    u32 idx = start / 64;

    u64 *p = bits + idx;
    u64  x = *p & (ALL_ONES_64 << (start % 64));
    if (x != 0) {
        return idx * 64 + count_trailing_zeros(x);
    }

    p++;

    while (*p == 0 && p != end) {
        p++;
    }

    if (p == end) {
        return BIT_NOT_FOUND;
    }

    return (p - bits) * 64 + count_trailing_zeros(*p);
}

