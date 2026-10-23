// App vslot `0x533100` miss path OBS `0x533207..0x533305` (254 B; join with hit):
//   build stack hashed-string "r_fps" (vt 0x15976a0+0x10)
//   call 0x21620 → obj; optional vcall +0x18 / +0x30 (hooks)
//   call 0xba40(0x2efe560, *0x2efe5c0, r12)
//   call 0xf590 ×2 (stack string dtors; both holders `{"r_fps", 5, 0, 0}`)
//   // join @ 0x533305 — NOT modelled past join
//
// Always-on on measured Init when show_areaInfo lookup misses (empty table).
// Prefer over hit @ 0x533305 (not taken) and over deepening 0x525080/0x2569d0
// (already hooked call sites; huge bodies).
//
// Claimed: miss control flow + "r_fps" pin + ba40/f590 call counts + both
// 0xf590 releases as exact inert no-ops (`+0x18` proven zero by a dominating store).
// NOT modelled: body of 0x21620 (449 B); vcall bodies; ba40 non-empty listeners;
// join @ 0x533305+ (see vslot_40_533100_join.*). PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include "shared_release_f590.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100MissRva = 0x533207;
inline constexpr std::size_t kVslot40533100MissSize = 254;  // through join @ 0x533305
inline constexpr std::uint64_t kVslot40MissJoinRva = 0x533305;
inline constexpr std::uint64_t kVslot40MissFind21620Rva = 0x21620;
inline constexpr std::uint64_t kVslot40MissBa40CellRva = 0x2efe560;
inline constexpr std::uint64_t kVslot40MissArgCellRva = 0x2efe5c0;
inline constexpr const char kVslot40MissNameRFps[] = "r_fps";

struct Vslot40_533100MissHooks {
    // Models 0x21620(stack_hashed_string*) → setting obj or null.
    void* (*find_21620)(void* hashed_name) = nullptr;
    // Models vcall [obj.vt+0x18] → int (expect 1 to keep).
    int (*vslot_18)(void* obj) = nullptr;
    // Models vcall [obj.vt+0x30] → int (nonzero keeps; then cleared in OBS).
    int (*vslot_30)(void* obj) = nullptr;
    // Models 0xba40(0x2efe560, *0x2efe5c0, r12). Null → no-op.
    void (*ba40)(void* cell_2efe560, void* arg, void* r12) = nullptr;
    // 0xf590(rdi = &holder) at 0x5332f4 and 0x533300. Original: two 0x20 B stack strings
    // `{"r_fps", 5, nullptr, nullptr}`; the `+0x18` control word is zero at both sites
    // (dominating store @ 0x53325f / 0x533246, whole-function sweep). Null => run the lifted
    // release; non-null => host override, whose result is reported instead.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590& holder) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_f590{};
};

struct Vslot40_533100MissResult {
    bool ok = false;
    int find_21620_calls = 0;
    int ba40_calls = 0;
    int dtor_f590_calls = 0;
    bool find_null = false;  // measured empty path
    // Measured 0xf590 effect: both holders have `+0x18 == 0`, so nothing is decremented,
    // destroyed, or freed.
    SharedReleaseF590Result release_f590_a{};
    SharedReleaseF590Result release_f590_b{};
};

// Process-wide stand-ins for OBS cells (ba40 args).
[[nodiscard]] void*& vslot40_miss_cell_2efe560() noexcept;
[[nodiscard]] void*& vslot40_miss_cell_2efe5c0() noexcept;

// Models miss path when show_areaInfo lookup returned null. Stops at join.
[[nodiscard]] Vslot40_533100MissResult vslot_40_533100_miss(
    const Vslot40_533100MissHooks& hooks = {}) noexcept;

}  // namespace sotc
