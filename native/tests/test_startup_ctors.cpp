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
        CHECK(r.ok() && r.completed == 27);
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
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 22, 48, 49, 51, 52, 53, 54, 56, 57, 58, 60, 61, 62, 63, 520, 538};
    CHECK(recovered_constructors().size() == 27);
    for (std::size_t i = 0; i < 27; ++i)
        CHECK(recovered_constructors()[i].original_index == expect[i]);
    for (std::size_t i = 1; i < recovered_constructors().size(); ++i)
        CHECK(recovered_constructors()[i - 1].original_index < recovered_constructors()[i].original_index);
    std::puts("startup_ctors: all checks passed");
    return 0;
}
