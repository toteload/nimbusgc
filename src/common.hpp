#pragma once

#include <stdint.h>
#include <intrin.h>

typedef  uint8_t  u8;
typedef  int32_t i32;
typedef uint32_t u32;
typedef  int64_t i64;
typedef uint64_t u64;

inline constexpr u64 kilobytes(u64 x) {
    return 1024ull * x;
}

inline constexpr u64 megabytes(u64 x) {
    return 1024ull * kilobytes(x);
}

inline u64 round_up_to_power_of_two(u64 x, u64 power_of_two) {
    u64 y = power_of_two - 1;
    return (x + y) & ~y;
}

inline i64 ptr_diff(void *a, void *b) {
    return static_cast<u8*>(a) - static_cast<u8*>(b);
}

inline void *ptr_offset(void *p, i64 offset) {
    return static_cast<u8*>(p) + offset;
}

// If x is 0, then the returned value is undefined
inline u32 count_trailing_ones(u64 x) {
    u32 idx;
    _BitScanForward64(reinterpret_cast<unsigned long*>(&idx), ~x);
    return idx;
}

// If x is 0, then the returned value is undefined
inline u32 count_trailing_zeros(u64 x) {
    u32 idx;
    _BitScanForward64(reinterpret_cast<unsigned long*>(&idx), x);
    return idx;
}

