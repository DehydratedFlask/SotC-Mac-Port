// App vslot `0x533100` join+ OBS `0x533305..0x533411` (268 B):
//   both miss and hit converge here (always-on after E423)
//   call 0x42390 (stack descriptor) → build "Position" → 0xf590
//   alloc 0x20 via 0x17070 → call 0x2779a0(obj, desc, 1)
//   replace *[self+0x3b60] (destroy previous if any)
//   // stop before next cluster @ 0x533411 (see vslot_40_533100_gfx_2ee2d00.*)
//
// Prefer over deepen 0x21620 (miss-only, pre-join) and 0x525080/0x2569d0
// (already hooked pre-branch). Prefer over empty-path migrate/non-null.
//
// Claimed: join control flow + "Position" pin + alloc/store at +0x3b60 + the stack
// `0xf590` release as an exact inert no-op (`+0x18` proven zero).
// NOT modelled: body of 0x2779a0; body of 0x42390 (hooked); gfx/shadowmodels
// past join; hit-only side effects before join. The per-element `0xf590` in the
// `[self+0x3b60]` destroy loop (0x5333e3) stays an unresolved host hook: the 0x50 B
// element's `+0x18` is heap state, not a proven-zero stack slot. PR #5 cite-only.
#pragma once

#include "allocator_front.hpp"
#include "shared_release_f590.hpp"

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
    // 0xf590(rdi = &holder) at 0x533399. Original: 0x20 B stack string
    // `{"Position", 8, nullptr, nullptr}`; `+0x18` is zero (dominating vpxor @ 0x533341).
    // Null => run the lifted release; non-null => host override, whose result is reported.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590& holder) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_f590{};
    // 0xf590 per element of the `[self+0x3b60]` vector at 0x5333e3. UNRESOLVED: the 0x50 B
    // element's `+0x18` is heap state written when the vector was built, so this must not be
    // wired as an inert zero-holder release. Null → no-op.
    void (*dtor_f590_element)(void* elem) = nullptr;
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
    int dtor_f590_count = 0;          // stack release + one per replaced element
    int release_f590_count = 0;       // stack "Position" release (OBS 1)
    int dtor_f590_element_count = 0;  // 0x5333e3 loop elements only
    bool alloc_failed = false;
    // Measured 0xf590 effect on the stack name: `+0x18 == 0`, so nothing is decremented,
    // destroyed, or freed.
    SharedReleaseF590Result release_f590{};
};

// Models join+ through Position install at self+0x3b60. Stops before 0x533411.
[[nodiscard]] Vslot40_533100JoinResult vslot_40_533100_join(
    AllocatorFront& alloc, void* self, const Vslot40_533100JoinHooks& hooks = {}) noexcept;

}  // namespace sotc
