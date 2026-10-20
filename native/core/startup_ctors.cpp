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
        {0xf4b510, 7, [](StartupState& s) noexcept { init_vec_neg036(s.neg036); }},
        {0xf3d9d0, 8, [](StartupState& s) noexcept { init_math_constants(s.math_d); }},
        {0xf30460, 9, [](StartupState& s) noexcept { init_xyz_mask_copy(s.xyz_mask_copy); }},
        {0xe10490, 48, [](StartupState& s) noexcept { init_trig_constants(s.trig); }},
        {0x196c90, 520, [](StartupState& s) noexcept { clear_flag32(s.flag32); }},
        {0x125990, 538, [](StartupState& s) noexcept { reset_scratch(s.scratch, s.scratch_slot); }},
    };
    return kSlots;
}

}  // namespace sotc
