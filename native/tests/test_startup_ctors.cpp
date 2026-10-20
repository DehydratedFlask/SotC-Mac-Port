#include <cstdio>
#include <cstdlib>

#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;
    {
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        st.scratch_slot = 5;
        st.flag32 = 0xA5A5A5A5u;
        auto r = run_constructors(recovered_constructors(), st);
        CHECK(r.ok() && r.completed == 91);
        CHECK(st.trig.cos_3_degrees > 0.9986f);
        CHECK(st.neg036.guard == 1 && st.neg036.lanes[0] == kNeg036Bits);
        CHECK(st.xyz_mask_copy == kXyzMaskBits && st.xyz_mask_b == kXyzMaskBits);
        CHECK(st.f4fd40.ones0 == ~0ull && st.f4fd40.epsilon_x4[0] == kEpsilonBits);
        CHECK(st.f4eba0.ones == ~0ull && st.f4eba0.q1 == 0xffffffff00000000ull);
        CHECK(st.f4b540[0] == 0x00 && st.f4b540[3] == 0x80 && st.f4b540.size() == 240);
        CHECK(st.vec_ef85e0 == kVecEf85e0Bits);
        CHECK(st.ea52f0[0] == 0xff && st.ea52f0[16] == 0x0c && st.ea52f0.size() == 184);
        CHECK(st.vec_e0b180 == kVecE0b180Bits);
        CHECK(st.vec_e085e0 == kVecE085e0Bits);
        CHECK(st.vec_e05320 == kVecE05320Bits);
        CHECK(st.vec_e026b0 == kVecE026b0Bits);
        CHECK(st.vec_dfd5f0 == kVecDfd5f0Bits);
        CHECK(st.vec_de7fb0 == kVecDe7fb0Bits);
        CHECK(st.vec_de6d20 == kVecDe6d20Bits);
        CHECK(st.vec_de1ad0 == kVecDe1ad0Bits);
        CHECK(st.vec_ddef00 == kVecDdef00Bits);
        CHECK(st.vec_dddc90 == kVecDddc90Bits);
        CHECK(st.vec_ddab00 == kVecDdab00Bits);
        CHECK(st.vec_dda990 == kVecDda990Bits);
        CHECK(st.vec_dd41e0.guard == 1 && st.vec_dd41e0.lanes[0] == kNeg036BitsDd41);
        CHECK(st.vec_dc4180 == kVecDc4180Bits);
        CHECK(st.vec_dc2d50 == kVecDc2d50Bits);
        CHECK(st.vec_dbb280 == kVecDbb280Bits);
        CHECK(st.vec_db93a0 == kVecDb93a0Bits);
        CHECK(st.vec_db7e60 == kVecDb7e60Bits);
        CHECK(st.vec_db5dd0 == kVecDb5dd0Bits);
        CHECK(st.vec_db3a70 == kVecDb3a70Bits);
        CHECK(st.vec_db3960 == kVecDb3960Bits);
        CHECK(st.vec_dac410 == kVecDac410Bits);
        CHECK(st.vec_d94cb0 == kVecD94cb0Bits);
        CHECK(st.vec_d90310 == kVecD90310Bits);
        CHECK(st.vec_d853a0 == kVecD853a0Bits);
        CHECK(st.vec_d84ee0 == kVecD84ee0Bits);
        CHECK(st.vec_d80e60 == kVecD80e60Bits);
        CHECK(st.vec_d7fe90 == kVecD7fe90Bits);
        CHECK(st.vec_d7fe40 == kVecD7fe40Bits);
        CHECK(st.vec_d7e640 == kVecD7e640Bits);
        CHECK(st.vec_d7c9d0 == kVecD7c9d0Bits);
        CHECK(st.vec_d7c4d0 == kVecD7c4d0Bits);
        CHECK(st.vec_d7c380 == kVecD7c380Bits);
        CHECK(st.vec_d7be60 == kVecD7be60Bits);
        CHECK(st.vec_d7bc60 == kVecD7bc60Bits);
        CHECK(st.vec_d7b930 == kVecD7b930Bits);
        CHECK(st.vec_d6e0f0 == kVecD6e0f0Bits);
        CHECK(st.vec_d6dbf0 == kVecD6dbf0Bits);
        CHECK(st.vec_d6d890 == kVecD6d890Bits);
        CHECK(st.vec_d6d280 == kVecD6d280Bits);
        CHECK(st.vec_d6a520 == kVecD6a520Bits);
        CHECK(st.vec_d62b00 == kVecD62b00Bits);
        CHECK(st.vec_d62690 == kVecD62690Bits);
        CHECK(st.vec_d623c0 == kVecD623c0Bits);
        CHECK(st.vec_d5fbe0 == kVecD5fbe0Bits);
        CHECK(st.vec_d47880 == kVecD47880Bits);
        CHECK(st.vec_d47420 == kVecD47420Bits);
        CHECK(st.vec_d47270 == kVecD47270Bits);
        CHECK(st.vec_d42b60 == kVecD42b60Bits);
        CHECK(st.vec_d40ea0 == kVecD40ea0Bits);
        CHECK(st.vec_d405a0 == kVecD405a0Bits);
        CHECK(st.vec_d40010 == kVecD40010Bits);
        CHECK(st.vec_d3b790 == kVecD3b790Bits);
        CHECK(st.vec_d3a6d0 == kVecD3a6d0Bits);
        CHECK(st.vec_d32720 == kVecD32720Bits);
        CHECK(st.vec_d2ffe0 == kVecD2ffe0Bits);
        CHECK(st.vec_d32b70 == kVecD32b70Bits);
        CHECK(st.vec_d2cb30 == kVecD2cb30Bits);
        CHECK(st.vec_d2c7e0 == kVecD2c7e0Bits);
        CHECK(st.vec_d20590 == kVecD20590Bits);
        CHECK(st.vec_d19bb0 == kVecD19bb0Bits);
        CHECK(st.vec_d19af0 == kVecD19af0Bits);
        CHECK(st.vec_d19930 == kVecD19930Bits);
        CHECK(st.vec_d17c10 == kVecD17c10Bits);
        CHECK(st.vec_d13300 == kVecD13300Bits);
        CHECK(st.vec_d0bde0 == kVecD0bde0Bits);
        CHECK(st.vec_d0bcd0 == kVecD0bcd0Bits);
        CHECK(st.vec_d0a250 == kVecD0a250Bits);
        CHECK(st.vec_d09160 == kVecD09160Bits);
        CHECK(st.vec_d082e0 == kVecD082e0Bits);
        CHECK(st.vec_d08240 == kVecD08240Bits);
        CHECK(st.vec_d06dd0 == kVecD06dd0Bits);
        CHECK(st.vec_d04ce0 == kVecD04ce0Bits);
        CHECK(st.vec_d00600 == kVecD00600Bits);
        CHECK(st.vec_b743c0 == kVecB743c0Bits);
        CHECK(st.vec_b52110 == kVecB52110Bits);
        CHECK(st.math.pi_guard == 1 && st.math_b.pi_guard == 1 && st.math_c.pi_guard == 1 && st.math_d.pi_guard == 1);
        CHECK(st.flag32.load() == 0);
        CHECK(st.scratch.back() == std::byte{0} && st.scratch_slot.load() == 0);
    }
    {
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        const auto slots = recovered_constructors();
        const CtorSlot order[] = {slots[0], {0xABCDEF, 1, nullptr}, slots[slots.size() - 1]};
        auto r = run_constructors(order, st);
        CHECK(!r.ok() && r.completed == 1 && r.blocked_on_rva == 0xABCDEF);
        CHECK(st.math.pi_guard == 1);
        CHECK(st.scratch.front() == std::byte{0x77});
    }
    {
        StartupState st;
        auto r = run_constructors({}, st);
        CHECK(r.ok() && r.completed == 0);
    }
    const std::uint32_t expect[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 22, 48, 49, 51, 52, 53, 54, 56, 57, 58, 60, 61, 62, 63,
        67, 68, 69, 71, 72, 73, 74, 75, 76, 77, 78, 79, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 96,
        97, 98, 99, 100, 101, 102, 103, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 118, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 282, 290, 520, 538};
    CHECK(recovered_constructors().size() == 91);
    for (std::size_t i = 0; i < 91; ++i)
        CHECK(recovered_constructors()[i].original_index == expect[i]);
    for (std::size_t i = 1; i < recovered_constructors().size(); ++i)
        CHECK(recovered_constructors()[i - 1].original_index < recovered_constructors()[i].original_index);
    std::puts("startup_ctors: all checks passed");
    return 0;
}
