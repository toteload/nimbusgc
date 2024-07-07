#include "allocator.hpp"
#include <algorithm>

void Allocator::init(u64 initial_size, u64 reserve_size) {
    mem.init(initial_size, reserve_size);
    markbit.resize(mem.committed_byte_count() / 512, 0);
}

u64 *Allocator::find_marked_allocation(u64 *start) {
    u64 *base   = static_cast<u64*>(mem.base);
    u32  offset = start - base;

    u64 *bits = markbit.data();
    u64 *end  = bits + markbit.size();
    u32  next = find_first_one_bit(bits, end, offset);

    if (next == BIT_NOT_FOUND) {
        return nullptr;
    }

    return base + next;
}

u64 *Allocator::find_free_block(u64 *start) {
    u64 *base   = static_cast<u64*>(mem.base);
    u32  offset = start - base;

    u64 *bits = markbit.data();
    u64 *end  = bits + markbit.size();
    u32  next = find_first_zero_bit(bits, end, offset);

    if (next == BIT_NOT_FOUND) {
        return nullptr;
    }

    return base + next;
}

void *Allocator::alloc_inner(u32 size) {
    size = round_up_to_power_of_two(size, ALLOC_GRANULARITY);

    u32 n = size + HEADER_SIZE;

    if (n > mem.free_byte_count()) {
        mem.grow(n);
        markbit.resize(mem.committed_byte_count() / 8, 0);
    }

    u64 *res = static_cast<u64*>(mem.used);
    *res = size / ALLOC_GRANULARITY;

    mem.used = ptr_offset(mem.used, n);

    return res + 1;
}

void Allocator::mark_inner(void **ref) {
    ptr.push_back(ref);

    u64 *allocation = static_cast<u64*>(*ref) - 1;

    u64 n     = *allocation;
    u64 start = allocation - static_cast<u64*>(mem.base);

    set_bits(markbit.data(), start, n + 1);
}

void Allocator::compact() {
    std::sort(ptr.begin(), ptr.end(), [](void** a, void** b) { return *a < *b; });

    void ***p   = ptr.data(); // Wow, three stars.
    void ***end = p + ptr.size();

    u64 *free   = find_free_block(static_cast<u64*>(mem.base));
    u64 *marked = find_marked_allocation(free);

    // If no marked allocation was found after the first free block, then
    // all allocations are already compact.
    if (!marked) {
        memset(markbit.data(), 0, markbit.size() * sizeof(u64));
        ptr.clear();
        return;
    }

    // Skip over all allocations that are already compacted.
    while (p != end && **p < marked) {
        p++;
    }

    while (marked) {
        while (p != end && **p == marked + 1) {
            **p = free + 1;
            p++;
        }

        u32 n = *marked + 1;
        memmove(free, marked, n * ALLOC_GRANULARITY);

        free += n;
        marked = find_marked_allocation(marked + n);
    }

    mem.used = free;

    memset(markbit.data(), 0, markbit.size() * sizeof(u64));
    ptr.clear();
}


