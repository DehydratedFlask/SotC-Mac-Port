#include "startup_ctors.hpp"

namespace sotc {

CtorRunResult run_constructors(std::span<const CtorSlot> in_order, StartupState& state) noexcept {
    CtorRunResult result;
    for (const CtorSlot& slot : in_order) {
        if (slot.run == nullptr) {
            result.blocked_on_rva = slot.original_rva;   // no stub-success: caller must treat as startup failure
            return result;
        }
        slot.run(state);
        ++result.completed;
    }
    return result;
}

std::span<const CtorSlot> recovered_constructors() noexcept {
    static constexpr CtorSlot kSlots[] = {
        {0xf6cad0, 0, [](StartupState& s) noexcept { init_math_constants(s.math); }},
        {0xf66640, 1, [](StartupState& s) noexcept { init_math_constants(s.math_b); }},
        {0xf53de0, 2, [](StartupState& s) noexcept { init_math_constants(s.math_c); }},
        {0xf50d70, 3, [](StartupState& s) noexcept { init_xyz_mask_copy(s.xyz_mask_b); }},
        {0xf4fd40, 4, [](StartupState& s) noexcept { init_f4fd40(s.f4fd40); }},
        {0xf4eba0, 5, [](StartupState& s) noexcept { init_f4eba0(s.f4eba0); }},
        {0xf4b540, 6, [](StartupState& s) noexcept { init_f4b540(s.f4b540); }},
        {0xf4b510, 7, [](StartupState& s) noexcept { init_vec_neg036(s.neg036); }},
        {0xf3d9d0, 8, [](StartupState& s) noexcept { init_math_constants(s.math_d); }},
        {0xf30460, 9, [](StartupState& s) noexcept { init_xyz_mask_copy(s.xyz_mask_copy); }},
        // Gap 10–19: heap/atexit wrappers (need native malloc contract; not stubbed).
        {0xef85e0, 20, [](StartupState& s) noexcept { init_vec_ef85e0(s.vec_ef85e0); }},
        {0xea52f0, 22, [](StartupState& s) noexcept { init_ea52f0(s.ea52f0); }},
        {0xe10490, 48, [](StartupState& s) noexcept { init_trig_constants(s.trig); }},
        {0xe0b180, 49, [](StartupState& s) noexcept { init_vec_e0b180(s.vec_e0b180); }},
        // Gap idx 50 (0xe094e0): peer-complete historically but sources not on origin/main yet.
        {0xe085e0, 51, [](StartupState& s) noexcept { init_vec_e085e0(s.vec_e085e0); }},
        {0xe05320, 52, [](StartupState& s) noexcept { init_vec_e05320(s.vec_e05320); }},
        {0xe026b0, 53, [](StartupState& s) noexcept { init_vec_e026b0(s.vec_e026b0); }},
        {0xdfd5f0, 54, [](StartupState& s) noexcept { init_vec_dfd5f0(s.vec_dfd5f0); }},
        // Gap idx 55.
        {0xde7fb0, 56, [](StartupState& s) noexcept { init_vec_de7fb0(s.vec_de7fb0); }},
        {0xde6d20, 57, [](StartupState& s) noexcept { init_vec_de6d20(s.vec_de6d20); }},
        {0xde1ad0, 58, [](StartupState& s) noexcept { init_vec_de1ad0(s.vec_de1ad0); }},
        // Gap idx 59.
        {0xddef00, 60, [](StartupState& s) noexcept { init_vec_ddef00(s.vec_ddef00); }},
        {0xdddc90, 61, [](StartupState& s) noexcept { init_vec_dddc90(s.vec_dddc90); }},
        {0xddab00, 62, [](StartupState& s) noexcept { init_vec_ddab00(s.vec_ddab00); }},
        {0xdda990, 63, [](StartupState& s) noexcept { init_vec_dda990(s.vec_dda990); }},
        // Gap idx 64–66.
        {0xdd41e0, 67, [](StartupState& s) noexcept { init_vec_dd41e0(s.vec_dd41e0); }},
        {0xdc4180, 68, [](StartupState& s) noexcept { init_vec_dc4180(s.vec_dc4180); }},
        {0xdc2d50, 69, [](StartupState& s) noexcept { init_vec_dc2d50(s.vec_dc2d50); }},
        // Gap idx 70 (peer lifting 0xdbfa60).
        {0xdbb280, 71, [](StartupState& s) noexcept { init_vec_dbb280(s.vec_dbb280); }},
        {0xdb93a0, 72, [](StartupState& s) noexcept { init_vec_db93a0(s.vec_db93a0); }},
        {0xdb7e60, 73, [](StartupState& s) noexcept { init_vec_db7e60(s.vec_db7e60); }},
        {0xdb5dd0, 74, [](StartupState& s) noexcept { init_vec_db5dd0(s.vec_db5dd0); }},
        {0xdb3a70, 75, [](StartupState& s) noexcept { init_vec_db3a70(s.vec_db3a70); }},
        {0xdb3960, 76, [](StartupState& s) noexcept { init_vec_db3960(s.vec_db3960); }},
        {0xdac410, 77, [](StartupState& s) noexcept { init_vec_dac410(s.vec_dac410); }},
        {0xd94cb0, 78, [](StartupState& s) noexcept { init_vec_d94cb0(s.vec_d94cb0); }},
        {0xd90310, 79, [](StartupState& s) noexcept { init_vec_d90310(s.vec_d90310); }},
        // Gap idx 80–82.
        {0xd853a0, 83, [](StartupState& s) noexcept { init_vec_d853a0(s.vec_d853a0); }},
        {0xd84ee0, 84, [](StartupState& s) noexcept { init_vec_d84ee0(s.vec_d84ee0); }},
        {0xd80e60, 85, [](StartupState& s) noexcept { init_vec_d80e60(s.vec_d80e60); }},
        {0xd7fe90, 86, [](StartupState& s) noexcept { init_vec_d7fe90(s.vec_d7fe90); }},
        {0xd7fe40, 87, [](StartupState& s) noexcept { init_vec_d7fe40(s.vec_d7fe40); }},
        {0xd7e640, 88, [](StartupState& s) noexcept { init_vec_d7e640(s.vec_d7e640); }},
        {0xd7c9d0, 89, [](StartupState& s) noexcept { init_vec_d7c9d0(s.vec_d7c9d0); }},
        {0xd7c4d0, 90, [](StartupState& s) noexcept { init_vec_d7c4d0(s.vec_d7c4d0); }},
        {0xd7c380, 91, [](StartupState& s) noexcept { init_vec_d7c380(s.vec_d7c380); }},
        {0xd7be60, 92, [](StartupState& s) noexcept { init_vec_d7be60(s.vec_d7be60); }},
        {0xd7bc60, 93, [](StartupState& s) noexcept { init_vec_d7bc60(s.vec_d7bc60); }},
        {0xd7b930, 94, [](StartupState& s) noexcept { init_vec_d7b930(s.vec_d7b930); }},
        // Gap idx 95 (0xd71f60) — NEEDS_PRINCIPAL.
        {0xd6e0f0, 96, [](StartupState& s) noexcept { init_vec_d6e0f0(s.vec_d6e0f0); }},
        {0xb743c0, 282, [](StartupState& s) noexcept { init_vec_b743c0(s.vec_b743c0); }},
        {0xb52110, 290, [](StartupState& s) noexcept { init_vec_b52110(s.vec_b52110); }},
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
    };
    return kSlots;
}

}  // namespace sotc
