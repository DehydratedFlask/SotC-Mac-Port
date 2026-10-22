// App vslot `0x533100` post-join gfx-cell prelude OBS `0x533411..0x533435` (36 B):
//   rbx = &cell_0x2ee2d00
//   call 0x6a5370(*cell)
//   call 0x6abcc0(*cell)
//   call 0x147e10(*cell, 0x2a2a2a2a)  // mov dword [rdi+0x40], esi; ret
//   // stop before shadowmodels.crmp path @ 0x533435
//
// Always-on after E424 join+. Prefer over deepen 0x2779a0 / 0x21620 /
// 0x525080 / 0x2569d0 (already hooked, not next sequential edge) and over
// empty-path migrate/non-null.
//
// Claimed: prelude control flow + cell pin + lifted 0x147e10 store.
// NOT modelled: bodies of 0x6a5370 / 0x6abcc0; shadowmodels.crmp load /
// vcall +0x18; shadowskels. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100GfxRva = 0x533411;
inline constexpr std::size_t kVslot40533100GfxSize = 36;
inline constexpr std::uint64_t kVslot40GfxEndRva = 0x533435;
inline constexpr std::uint64_t kVslot40GfxCellRva = 0x2ee2d00;
inline constexpr std::uint32_t kVslot40GfxMarker = 0x2a2a2a2au;
inline constexpr std::uint64_t kVslot40GfxMarkerOff = 0x40;
// Stand-in object must cover +0x40 dword store.
inline constexpr std::size_t kVslot40GfxObjBytes = 0x48;

struct Vslot40_533100GfxHooks {
    // Models 0x6a5370(obj). Null → no-op.
    void (*call_6a5370)(void* obj) = nullptr;
    // Models 0x6abcc0(obj). Null → no-op.
    void (*call_6abcc0)(void* obj) = nullptr;
};

struct Vslot40_533100GfxResult {
    bool ok = false;
    void* obj = nullptr;
    int call_6a5370_count = 0;
    int call_6abcc0_count = 0;
    bool marker_stored = false;
    std::uint32_t marker_value = 0;
};

// Process-wide stand-in for OBS cell 0x2ee2d00 (points into owned obj buffer).
[[nodiscard]] void*& vslot40_gfx_cell_2ee2d00() noexcept;

// Models post-join gfx prelude. Stops before shadowmodels @ 0x533435.
[[nodiscard]] Vslot40_533100GfxResult vslot_40_533100_gfx_2ee2d00(
    const Vslot40_533100GfxHooks& hooks = {}) noexcept;

// Lifted OBS 0x147e10: mov dword [rdi+0x40], esi; ret.
void store_u32_at_40_147e10(void* obj, std::uint32_t value) noexcept;

}  // namespace sotc
