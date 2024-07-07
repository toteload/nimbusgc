#include "catch_amalgamated.hpp"
#include "bitmap.hpp"

TEST_CASE("Bitmap operations", "[bitmap]") {
    u64 bits[4] = { 0 };

    SECTION("setting range of bits spanning multiple u64 and clearing all") {
        set_bits(bits, 63, 65);

        REQUIRE(bits[0] == 0x8000'0000'0000'0000);
        REQUIRE(bits[1] == 0xffff'ffff'ffff'ffff);
        REQUIRE(bits[2] == 0x0000'0000'0000'0001);
        REQUIRE(bits[3] == 0x0000'0000'0000'0000);
    }

    SECTION("setting range of bits within a u64") {
        set_bits(bits, 12, 3);

        REQUIRE(bits[0] == 0x0000'0000'0000'7000);
    }

    SECTION("finding the first one bit and the first zero bit after that") {
        set_bits(bits, 63, 65);

        u64 *end = bits + 4;

        REQUIRE(find_first_one_bit( bits, end,  0) ==  63);
        REQUIRE(find_first_zero_bit(bits, end, 63) == 129);
    }
}
