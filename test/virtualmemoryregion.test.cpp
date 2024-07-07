#include "catch_amalgamated.hpp"
#include "virtualmemoryregion.hpp"

TEST_CASE("VirtualMemoryRegion operations", "[virtualmemoryregion]") {
    VirtualMemoryRegion region;
    region.init(kilobytes(4), kilobytes(8));

    REQUIRE(region.page_size == kilobytes(4));

    SECTION("has the expected amount of free and used bytes") {
        region.used = ptr_offset(region.used, kilobytes(1));

        REQUIRE(region.committed_byte_count() == kilobytes(4));
        REQUIRE(region.free_byte_count()      == kilobytes(3));
        REQUIRE(region.used_byte_count()      == kilobytes(1));

        REQUIRE(region.reserved_byte_count()  == kilobytes(8));
    }

    SECTION("grows by an appropriate amount of pages") {
    }
}
