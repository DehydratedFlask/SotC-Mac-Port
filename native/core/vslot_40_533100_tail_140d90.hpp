// App vslot `0x533100` tail call-site OBS `0x53403c..0x534052` (22 B):
//   mov esi, 9; mov rdi, r15 (= self+0x3cc8); call 0x140d90
//   lea rdi, [rbp-0x70]; call 0xf590
//   // stop before stack-check / epilogue @ 0x534052
//     (see vslot_40_533100_stackchk.*)
//
// Always-on after E443 cp3shadowprecache attach. Prefer over success atexit
// (already skipped in attach) and over deepen 0x14c2f0 / 0x6a5370 (already
// hooked earlier). Prefer over full body of 0x140d90 (143 B; mutex/atexit).
// Prefer over empty-path migrate/non-null.
//
// Claimed: call-site control flow + arg pin (slot=self+0x3cc8, esi=9) + the stack
// `0xf590` release as an exact inert no-op (`+0x18` proven zero).
// NOT modelled: body of 0x140d90. Stack-check: see stackchk.*. Bodies of
// 0x14c2f0 / 0x6a5370. The recovered `+0x20 = 1.0f` is carried in the temp; `+0x28`/
// `+0x30` stay unmodelled because `0xf590` reads only `+0x00`/`+0x18`. PR #5 cite-only.
#pragma once

#include "resource_pool.hpp"
#include "shared_release_f590.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Tail140d90Rva = 0x53403c;
inline constexpr std::size_t kVslot40533100Tail140d90Size = 22;
inline constexpr std::uint64_t kVslot40Tail140d90EndRva = 0x534052;
inline constexpr std::uint64_t kVslot40Tail140d90SlotOff = 0x3cc8;
inline constexpr int kVslot40Tail140d90Arg = 9;
inline constexpr const char kVslot40Tail140d90NamePosition[] = "Position";

struct Vslot40_533100Tail140d90Hooks {
    // Production path: the persistent site-9 slot. When non-null the lifted `lock_resource_ref`
    // runs with priority 9 (the `0x140d90` observable), instead of `call_140d90`.
    LockedResourceRef* slot = nullptr;
    ResourcePoolHooks pool_hooks{};
    // Models 0x140d90(slot_obj, arg). Null → no-op.
    void (*call_140d90)(void* slot_obj, int arg) = nullptr;
    // 0xf590(rdi = &holder) at 0x53404d. Original: 0x38 B stack temp whose first 0x20 B are
    // `{"Position", 8, nullptr, nullptr}`; `+0x18` is zero (dominating vpxor @ 0x533341).
    // Null => run the lifted release; non-null => host override, whose result is reported.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590& holder) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_f590{};
};

struct Vslot40_533100Tail140d90Result {
    bool ok = false;
    void* slot_obj = nullptr;
    int arg = 0;
    int call_140d90_count = 0;
    bool lock_ok = false;
    std::uint32_t state_after = 0;
    std::uint32_t deps_after = 0;
    int dtor_f590_count = 0;
    // Measured 0xf590 effect: the holder's `+0x18` is zero, so nothing is decremented,
    // destroyed, or freed.
    SharedReleaseF590Result release_f590{};
};

[[nodiscard]] Vslot40_533100Tail140d90Result vslot_40_533100_tail_140d90(
    void* self, const Vslot40_533100Tail140d90Hooks& hooks = {}) noexcept;

}  // namespace sotc
