// App vslot `0x533100` join+ OBS `0x533305..0x533411` (268 B):
//   both miss and hit converge here (always-on after E423)
//   call 0x42390 (stack descriptor) → build "Position" → 0xf590
//   alloc 0x20 via 0x17070 → call 0x2779a0(obj, desc, 1)
//   replace *[self+0x3b60] (destroy previous if any)
//   // stop before next cluster @ 0x533411 (0x6a5370 / shadowmodels…)
//
// Prefer over deepen 0x21620 (miss-only, pre-join) and 0x525080/0x2569d0
// (already hooked pre-branch). Prefer over empty-path migrate/non-null.
//
// Claimed: join control flow + "Position" pin + alloc/store at +0x3b60.
// NOT modelled: body of 0x2779a0; body of 0x42390 (hooked); shadowmodels /
// 0x6a5370 cluster; hit-only side effects before join. PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100JoinRva = 0x533305;
inline constexpr std::size_t kVslot40533100JoinSize = 268;  // through store @ 0x53340a
inline constexpr std::uint64_t kVslot40JoinEndRva = 0x533411;
inline constexpr std::uint64_t kVslot40JoinSlotOff = 0x3b60;
inline constexpr std::size_t kVslot40JoinPosObjectBytes = 0x20;
inline constexpr const char kVslot40JoinNamePosition[] = "Position";

struct Vslot40_533100JoinHooks {
    // Models 0x42390(stack_desc*). Null → no-op.
    void (*call_42390)(void* stack_desc) = nullptr;
    // Models 0x2779a0(obj, desc, flag). Null → memset obj zero.
    void (*ctor_2779a0)(void* obj, void* desc, int flag) = nullptr;
    // Models 0xf590 dtor on replaced vector elems / stack name. Null → no-op.
    void (*dtor_f590)(void* obj) = nullptr;
    // Models 0x170c0 delete[] of previous vec begin. Null → AllocatorFront::delete_array.
    void (*delete_array_170c0)(void* p) = nullptr;
    // Models 0x170b0 delete of previous holder. Null → AllocatorFront::delete_.
    void (*delete_170b0)(void* p) = nullptr;
};

struct Vslot40_533100JoinResult {
    bool ok = false;
    void* pos_obj = nullptr;
    void* prev_slot = nullptr;
    bool replaced = false;
    bool destroyed_prev = false;
    int call_42390_count = 0;
    int ctor_2779a0_count = 0;
    int dtor_f590_count = 0;
    bool alloc_failed = false;
};

// Models join+ through Position install at self+0x3b60. Stops before 0x533411.
[[nodiscard]] Vslot40_533100JoinResult vslot_40_533100_join(
    AllocatorFront& alloc, void* self, const Vslot40_533100JoinHooks& hooks = {}) noexcept;

}  // namespace sotc
