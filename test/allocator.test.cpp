#include "catch_amalgamated.hpp"
#include "allocator.hpp"

TEST_CASE("Allocator operations", "[allocator]") {
    Allocator allocator;
    allocator.init(kilobytes(4), kilobytes(16));

    SECTION("building a linked list and dropping the first half") {
        struct Node {
            Node *next;
            u64 payload;
        };

        Node *root = allocator.alloc<Node>();
        root->payload = 0;

        Node *prev = root;

        // Create a linked list of 8 elements holding a payload with their position in the list.
        for (u32 i = 1; i < 8; i++) {
            Node *current = allocator.alloc<Node>();        
            *current = {
                .next = nullptr,
                .payload = i,
            };
            prev->next = current;
            prev = current;
        }

        REQUIRE(allocator.bytes_used() == 192);

        // Find the node with payload 4
        Node *p = root;
        while (p->payload < 4) {
            p = p->next;
        }

        // `start` is going to be the new root
        Node *start = p;
        allocator.mark(&start);

        // Walk the rest of the list and mark each node
        Node **at = &p->next;
        while (*at) {
            allocator.mark(at);
            at = &(**at).next;
        }

        Node *precompaction_start = start;

        allocator.compact();

        REQUIRE(allocator.bytes_used() == 96);
        REQUIRE(start < precompaction_start);

        u64 expected = 4;
        p = start;
        while (p) {
            REQUIRE(p->payload == expected);
            expected++;
            p = p->next;
        }
    }
}
