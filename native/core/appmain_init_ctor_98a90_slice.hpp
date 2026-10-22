// Measured Init path: prologue early + tail using lifted 0x3be050 (header+0x3be930+ensure+
// 0x80 grow+null-head list+tsc) → base → registry. Tip AllocatorFront+FakeHeap.
// PR #5 cite-only. NOT modelled: ensure migrate / 0xf590 / 0xf8f860 PLT / non-null list;
// debug gates / 0x1ab550 / vslot / 0xf08.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "ctor_98a90_3be050.hpp"
#include "input_table_551140.hpp"
#include "prologue_255180.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitCtor98a90SliceResult {
    Prologue255180Result early{};
    Prologue255180TailResult tail{};
    Ctor98a90_3be050Result ctor{};
    bool base_ok = false;
    std::uint32_t flag_after_base = 0;
    AppmainInitRemainderSliceResult remainder{};
};

[[nodiscard]] AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c);

}  // namespace sotc
