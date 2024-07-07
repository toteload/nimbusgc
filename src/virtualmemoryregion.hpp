#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#undef VC_EXTRALEAN

#include "common.hpp"

struct VirtualMemoryRegion {
    void *base;
    void *used;
    void *committed;
    void *end;

    u32 page_size;

    inline void init(u64 initial_commit_size, u64 reserve_size);
    inline void grow(u64 required_free);

    u64 committed_byte_count() { return ptr_diff(committed, base); }
    u64 free_byte_count()      { return ptr_diff(committed, used); }
    u64 used_byte_count()      { return ptr_diff(     used, base); }
    u64 reserved_byte_count()  { return ptr_diff(      end, base); }
};

void VirtualMemoryRegion::init(u64 initial_commit_size, u64 reserve_size) {
    SYSTEM_INFO info = {};
    GetSystemInfo(&info);

    page_size = info.dwPageSize;

    initial_commit_size = round_up_to_power_of_two(initial_commit_size, page_size);
    reserve_size        = round_up_to_power_of_two(reserve_size,        page_size);

    void *p_reserve = VirtualAlloc(     NULL,        reserve_size, MEM_RESERVE, PAGE_READWRITE);
    void *p         = VirtualAlloc(p_reserve, initial_commit_size, MEM_COMMIT,  PAGE_READWRITE);

    base      = p;
    used      = p;
    committed = ptr_offset(p, initial_commit_size);
    end       = ptr_offset(p, reserve_size);
}

void VirtualMemoryRegion::grow(u64 required_free) {
    u32 extra = round_up_to_power_of_two(required_free - free_byte_count(), page_size);
    committed = VirtualAlloc(committed, extra, MEM_COMMIT, PAGE_READWRITE);
}

