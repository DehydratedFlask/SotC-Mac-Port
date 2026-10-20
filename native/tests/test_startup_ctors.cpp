#include <cstdio>
#include <cstdlib>

#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;
    {   // Recovered subset runs and produces the specified state.
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        st.scratch_slot = 5;
        st.flag32 = 0xA5A5A5A5u;
        auto r = run_constructors(recovered_constructors(), st);
        CHECK(r.ok() && r.completed == 10);
        CHECK(st.trig.cos_3_degrees > 0.9986f);
        CHECK(st.neg036.guard == 1 && st.neg036.lanes[0] == kNeg036Bits);
        CHECK(st.xyz_mask_copy == kXyzMaskBits && st.xyz_mask_b == kXyzMaskBits);
        CHECK(st.math.pi_guard == 1 && st.math_b.pi_guard == 1 && st.math_c.pi_guard == 1 && st.math_d.pi_guard == 1);
        CHECK(st.flag32.load() == 0);
        CHECK(st.scratch.back() == std::byte{0} && st.scratch_slot.load() == 0);
    }
    {   // An unrecovered constructor stops the run visibly; later slots do not execute.
        StartupState st;
        st.scratch.fill(std::byte{0x77});
        const auto slots = recovered_constructors();
        const CtorSlot order[] = {slots[0], {0xABCDEF, 1, nullptr}, slots[slots.size() - 1]};
        auto r = run_constructors(order, st);
        CHECK(!r.ok() && r.completed == 1 && r.blocked_on_rva == 0xABCDEF);
        CHECK(st.math.pi_guard == 1);
        CHECK(st.scratch.front() == std::byte{0x77});     // 0x125990 stand-in never ran
    }
    {   // Empty list is trivially complete.
        StartupState st;
        auto r = run_constructors({}, st);
        CHECK(r.ok() && r.completed == 0);
    }
    // Contiguous early run 0..3 recovered; gap at 4..6; then 7..9.
    CHECK(recovered_constructors()[0].original_index == 0);
    CHECK(recovered_constructors()[1].original_index == 1);
    CHECK(recovered_constructors()[2].original_index == 2);
    CHECK(recovered_constructors()[3].original_index == 3);
    CHECK(recovered_constructors()[4].original_index == 7);
    CHECK(recovered_constructors()[5].original_index == 8);
    CHECK(recovered_constructors()[6].original_index == 9);
    CHECK(recovered_constructors()[7].original_index == 48);
    CHECK(recovered_constructors()[8].original_index == 520);
    CHECK(recovered_constructors()[9].original_index == 538);
    for (std::size_t i = 1; i < recovered_constructors().size(); ++i)
        CHECK(recovered_constructors()[i - 1].original_index < recovered_constructors()[i].original_index);
    std::puts("startup_ctors: all checks passed");
    return 0;
}
