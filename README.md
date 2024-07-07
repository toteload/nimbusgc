# Nimbus Garbage Collector

Nimbus is a small, tracing, compacting garbage collector (currently Windows only).
I made it because I wanted to make a garbage collector. 
Maybe someday I will use it for an interpreted language.
It is not thoroughly tested so there are likely some bugs 🐛
There is a test showing usage of the allocator to create a linked list, marking the desired
allocations and the rest getting collected.

## Architecture

### Memory

All allocations are in one contiguous region of memory that grows if more memory is needed.
This chunk is managed using virtual memory functions. 
Using virtual memory we can grow the region without having to move our memory.
This is important, because if we move our region then all pointers in allocations pointing to
other allocations will be invalidated.

Allocations returned by the allocator are always 8 byte aligned and the allocation granularity is 8 bytes.
Each allocation has an overhead of 8 bytes in the form of a header placed before the returned pointer.
This header currently only stores the size of the allocation.
Allocations are always taken from the back of the region.

### Marking allocations

All live allocations must be marked by the user.
This includes recursively walking down objects and marking their internal allocations.
The allocator itself is unaware of the types of the allocations.
A bitmap is used to keep track of what parts of memory are marked.
Each bit in this bitmap corresponds to a block of 8 bytes.
When marking an allocation the size of the allocation is taken and the corresponding bits in a bitmap are set.

To mark an allocation, a pointer to a pointer pointing to the allocation must be provided.
This pointer is stored by the allocator to update its content if the underlying allocation is moved during compaction.

### Compaction

During compaction all live allocations are moved to the start of the region.
Each live allocation has at least one pointer pointing to it.
If the allocation is moved, then all pointers pointing to it are updated as well.

During compaction we scan through the region from beginning to end.
We can quickly find marked allocations and free blocks using the bitmap.
We can also sort our array of pointers by pointed to allocation address to avoid having to search for pointers associated with an allocation.
This results in only looping through our list of pointers once after sorting.

