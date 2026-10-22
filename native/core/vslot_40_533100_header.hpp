// App vslot `0x533100` bounded header OBS `0x533100..0x5331cd` (205 B):
//   edi = 0x1c0; call 0x17070          // alloc
//   call 0x493230(obj)                 // ctor (490 B — NOT modelled)
//   lea cell → 0x2ef8c08; cmp/swap:
//     if prev != neu && prev: inlined destroy (list@+0x90 / 0x3b440×2 / 0x170b0)
//     store neu → cell
//   // remainder of 3956 B body NOT modelled (uses 0x2efc9e0, 0x525080, …)
//
// Always-on on measured Base Initialize path (vcall [vt+0x40] after mid 0x255340).
// Prefer over non-null list (measured null-head skips) and ensure migrate
// (empty begin==end path skips 0x1da30/0x170c0/0xf590).
//
// Claimed: header install control flow + alloc size + cell pin + callee sites.
// NOT modelled: full 0x533100 body; 0x493230 / inlined destroy / 0x3b440 bodies;
// ensure migrate; non-null list; heap/leaf-vec; idx 95. PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Rva = 0x533100;
inline constexpr std::size_t kVslot40533100FullSize = 3956;
inline constexpr std::size_t kVslot40533100HeaderSize = 205;  // through store @ 0x5331ca
inline constexpr std::size_t kVslot40533100ObjectBytes = 0x1c0;
inline constexpr std::uint64_t kVslot40533100CellRva = 0x2ef8c08;
inline constexpr std::uint64_t kVslot40533100CtorRva = 0x493230;

struct Vslot40_533100HeaderHooks {
    // Models 0x493230(rdi=obj). Null → zero-fill object (safe delete_ on replace).
    void (*ctor_493230)(void* obj) = nullptr;
    // Models inlined prev destroy OBS 0x533155..0x5331c5. Null → delete_ only.
    void (*destroy_prev)(void* prev) = nullptr;
};

struct Vslot40_533100HeaderResult {
    bool ok = false;
    bool alloc_failed = false;
    bool reused_same = false;
    bool destroyed_prev = false;
    bool ctor_ran = false;
    void* obj = nullptr;
};

// Process-wide cell modelling original qword @ 0x2ef8c08.
[[nodiscard]] void*& vslot40_cell_2ef8c08() noexcept;

void destroy_vslot40_at_2ef8c08(AllocatorFront& alloc,
                                const Vslot40_533100HeaderHooks& hooks = {}) noexcept;

// Header-only: install into cell; does not run the remainder of 0x533100.
// `self`/`arg` unused in header (OBS saves them for post-header body).
[[nodiscard]] Vslot40_533100HeaderResult vslot_40_533100_header(
    AllocatorFront& alloc, void* self, void* arg,
    const Vslot40_533100HeaderHooks& hooks = {}) noexcept;

}  // namespace sotc
