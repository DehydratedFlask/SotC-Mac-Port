// Retail call entry point: frame + guest window -> native model -> rax.
//
// `call_resolved` is the single place where a guest call frame becomes a native call:
//   1. look the target up in `retail_abi` (unknown target fails visibly);
//   2. marshal its cells from guest memory through `retail_cells` (out-of-range fails visibly);
//   3. invoke the model with the caller's hooks;
//   4. write the result back into `frame.rax`.
//
// Only targets with both an ABI row and a cell adapter are dispatchable today: `0x20d60`, `0x358c0`,
// `0x13d20`, `0x187c0`. Anything else returns `unknown-target` rather than guessing a ABI.
#pragma once

#include "guest_memory.hpp"
#include "retail_abi.hpp"
#include "retail_cells.hpp"

#include <cstdint>

namespace sotc {

struct RetailCallContext {
    // 0x20d60 hooks and inputs.
    void (*tail_20d60)(void* self, void* global) = nullptr;
    std::uint64_t global_20d60 = 0;  // guest address of the 0x2dfcbc8 cell, 0 = none

    // 0x187c0 / 0x2687b0 / 0x1bf710 hooks and allocation contract (each teardown frees `this`).
    void (*pre_187c0)(void* self) = nullptr;
    void (*pre_2687b0)(void* self) = nullptr;
    void (*pre_1bf710)(void* self) = nullptr;
    const AllocDispatchContract* alloc = nullptr;

    // 0xe16a00: dual vcall, both receive (self, argument).
    void (*slot18_e16a00)(void* self, void* argument) = nullptr;
    void (*slot20_e16a00)(void* self, void* argument) = nullptr;

    // 0xddf4b0: pre-pass, manager fetch, and the tail vcall through the manager.
    void (*pre_ddf4b0)(void* self) = nullptr;
    void* (*fetch_manager_ddf4b0)() = nullptr;
    void (*tail_ddf4b0)(void* manager, void* self) = nullptr;

    // 0xa460f0: teardown with one helper vcall, a post-pass, and the shared alloc contract.
    void (*helper_a460f0)(void* helper) = nullptr;
    void (*post_a460f0)(void* self) = nullptr;
};

enum class RetailCallStatus : std::uint8_t {
    ok = 0,
    unknown_target,
    no_window,
    out_of_range,
    model_failed,
};

const char* retail_call_status_name(RetailCallStatus status) noexcept;

struct RetailCallOutcome {
    bool ok = false;
    RetailCallStatus status = RetailCallStatus::unknown_target;
    std::uint64_t rax = 0;  // only meaningful when ok
};

// Dispatches `rva` with the arguments in `frame` (rdi/rsi/rdx/rcx/r8/r9) against `window`.
[[nodiscard]] RetailCallOutcome call_resolved(std::uint64_t rva, RetailFrame& frame,
                                              GuestWindow& window,
                                              const RetailCallContext& context = {}) noexcept;

}  // namespace sotc
