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

// Guest stack arena for targets whose retail body uses its own frame. The stack grows down from
// `top`; `size` is the number of bytes available below it. Addresses are guest addresses and are
// resolved through the caller's `GuestWindow`.
struct RetailStack {
    std::uint64_t top = 0;
    std::uint64_t size = 0;
};

// Allocates `bytes` (aligned) below `sp`. Returns false on overflow, leaving `sp` untouched.
[[nodiscard]] bool retail_stack_alloc(const RetailStack& stack, std::uint64_t& sp,
                                      std::uint64_t bytes, std::uint64_t alignment,
                                      std::uint64_t& address) noexcept;

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

    // 0xe24900: sub-dispatch hooks.
    void* (*slot28_e24900)(void* self) = nullptr;
    std::uint32_t (*slot48_e24900)(void* self) = nullptr;
    void* (*sub40_e24900)(void* sub, void* a, std::uint32_t index) = nullptr;
    std::int32_t (*status_e24900)(void* result, void* self) = nullptr;
    void (*release_e24900)(void* result) = nullptr;

    // 0xe26f90: status gate with host scratch storage (no guest stack mapping yet).
    std::uint64_t (*slot78_e26f90)(void* self) = nullptr;
    std::int32_t (*slot90_e26f90)(void* self, const char* name, void* buffer) = nullptr;
    std::int32_t (*helper_e26f90)(void* self, void* arg2, std::int32_t arg3, void* buffer,
                                  void* local) = nullptr;
    RetailStack stack{};

    // 0xe1a5c0: container scan hooks.
    const char* (*element_key_e1a5c0)(void* element) = nullptr;
    std::int32_t (*element_value_e1a5c0)(void* element) = nullptr;
    std::int32_t (*element_match_e1a5c0)(void* element) = nullptr;

    // 0xe19980: alignment-gated paths plus host scratch storage.
    void (*slow_e19980)(void* element, void* buffer) = nullptr;
    void* slow_buffer_e19980 = nullptr;
    void (*fast_e19980)(void* arg3, void* arg2, void* arg4, void* arg5) = nullptr;
    const void* const* store_value_e19980 = nullptr;

    // 0xe21420: decision-contract hooks.
    void* (*lookup_e21420)(void* provider, const char* name) = nullptr;
    std::int32_t (*state_e21420)(void* object) = nullptr;
    std::uint64_t (*kind_e21420)(void* object) = nullptr;
    std::int32_t (*count_e21420)(void* provider) = nullptr;
    const char* (*name_e21420)(void* provider, std::int32_t index) = nullptr;
    void (*missing_e21420)(void* provider) = nullptr;
    void (*found_e21420)(void* provider, std::int32_t index) = nullptr;
};

enum class RetailCallStatus : std::uint8_t {
    ok = 0,
    unknown_target,
    no_window,
    out_of_range,
    model_failed,
    stack_unavailable,
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
