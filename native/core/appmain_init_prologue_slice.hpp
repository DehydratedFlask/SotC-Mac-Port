// Measured Init path: prologue 0x255180 early fragment → base Initialize → remainder → registry.
// Uses tip AllocatorFront+FakeHeap. PR #5 cite-only. Remainder of 0x255180 NOT modelled.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "input_table_551140.hpp"
#include "prologue_255180.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitPrologueSliceResult {
    Prologue255180Result prologue{};
    bool base_ok = false;
    std::uint32_t flag_after_base = 0;  // expect 0 after orchestrator xchg(0)
    AppmainInitRemainderSliceResult remainder{};
};

// Run early prologue (installs flag cell), then base_initialize with prologue hook null,
// then E406 remainder → E405 registry.
[[nodiscard]] AppmainInitPrologueSliceResult run_appmain_init_prologue_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& prologue_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c);

}  // namespace sotc
