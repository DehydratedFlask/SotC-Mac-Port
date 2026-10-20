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
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
    };
    return kSlots;
}

}  // namespace sotc
