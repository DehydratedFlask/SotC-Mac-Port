// Measured Init path: prologue early + tail (through app+0x3b40) → base → remainder → registry.
// Uses tip AllocatorFront+FakeHeap. PR #5 cite-only.
// NOT modelled: debug gates after 0x2552cd; bodies of 0x3be050/0x3be390/0x1ab550.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "input_table_551140.hpp"
#include "prologue_255180.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitPrologueTailSliceResult {
    Prologue255180Result early{};
    Prologue255180TailResult tail{};
    bool base_ok = false;
    std::uint32_t flag_after_base = 0;
    AppmainInitRemainderSliceResult remainder{};
};

[[nodiscard]] AppmainInitPrologueTailSliceResult run_appmain_init_prologue_tail_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const BaseInitialize255130Hooks& base_hooks, void* setter_obj, std::uint8_t flag_in,
    std::uint8_t set_a, std::uint8_t set_b, std::uint8_t set_c);

}  // namespace sotc
