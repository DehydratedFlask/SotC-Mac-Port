// Measured Init early edge: 0xf08 singleton install → base Initialize 0x255130 →
// trivial remainder → registry.
//
// Static path (OBS): Init 0x531d60 @ 0x531d84 installs the 0xf08 singleton into
// cell 0x2efc9e0, then @ 0x531dda calls base Initialize on the app object; on
// success continues through trivial leaves (E406) and registry fragment (E405).
// Injectable hooks for unmodelled callees (0x523bd0/0x525b40; 0x255180 / 0x255340 /
// vslots 0x534130 / 0x533100). Tip AllocatorFront+FakeHeap. PR #5 heap cite-only.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "input_table_551140.hpp"
#include "singleton_f08_531d84.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitBaseInitializeSliceResult {
    SingletonF08Result singleton{};
    bool base_ok = false;
    bool base_fail_visible = false;  // vslot_30 false path exercised when requested
    std::uint32_t flag_prev = 0;     // previous dword at flag_obj+0x20 before xchg(0)
    std::uint32_t flag_after = 0;
    AppmainInitRemainderSliceResult remainder{};
};

// Install 0xf08 singleton, then base_initialize_255130; on success run remainder.
// `flag_obj` is installed into base_init_flag_cell_2ea4718 for the 0x357f0 step.
[[nodiscard]] AppmainInitBaseInitializeSliceResult run_appmain_init_base_initialize_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const BaseInitialize255130Hooks& hooks, void* flag_obj, void* setter_obj,
    std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c,
    const SingletonF08Hooks& singleton_hooks = {});

}  // namespace sotc
