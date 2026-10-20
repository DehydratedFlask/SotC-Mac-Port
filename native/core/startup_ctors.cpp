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
        // Gap idx 64–67 (0xdd41e0 incomplete API on origin).
        {0xdc4180, 68, [](StartupState& s) noexcept { init_vec_dc4180(s.vec_dc4180); }},
        {0xdc2d50, 69, [](StartupState& s) noexcept { init_vec_dc2d50(s.vec_dc2d50); }},
        // Gap idx 70 (peer lifting 0xdbfa60).
        {0xdbb280, 71, [](StartupState& s) noexcept { init_vec_dbb280(s.vec_dbb280); }},
        {0xb743c0, 282, [](StartupState& s) noexcept { init_vec_b743c0(s.vec_b743c0); }},
        {0xb52110, 290, [](StartupState& s) noexcept { init_vec_b52110(s.vec_b52110); }},
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
    };
    return kSlots;
}

}  // namespace sotc
