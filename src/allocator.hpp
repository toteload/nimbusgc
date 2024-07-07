#pragma once

#include "common.hpp"
#include "bitmap.hpp"
#include "virtualmemoryregion.hpp"

#include <vector>

constexpr u32 HEADER_SIZE       = sizeof(u64);
constexpr u32 ALLOC_GRANULARITY = sizeof(u64);

class Allocator {
    VirtualMemoryRegion mem;
    std::vector<u64>    markbit;
    std::vector<void**> ptr;

    u64 *find_marked_allocation(u64 *start);
    u64 *find_free_block(u64 *start);

    // Allocation granularity is 8 bytes
    // Returned memory is always 8 byte aligned
    void *alloc_inner(u32 size);
    void  mark_inner(void **ref);

public:
    void init(u64 initial_size, u64 reserve_size);

    template<typename T>
    inline T *alloc(u32 count=1) {
        return static_cast<T*>(alloc_inner(count * sizeof(T)));
    }

    template<typename T>
    void mark(T **ref) {
        mark_inner(reinterpret_cast<void**>(ref));
    }

    void compact();

    u64 bytes_used() { return mem.used_byte_count(); }
};


