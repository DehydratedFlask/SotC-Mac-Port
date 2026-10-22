// Base Initialize mid `0x255340` (712 B; debug-menu registration on success path).
//
// Always-on on measured Init path after vslot 0x534130 succeeds, before vslot 0x533100.
// Prefer over bounded 0x533100 header / 0xf08 / ensure migrate / non-null list.
//
// OBS:
//   vcall [self.vt+0x50]
//   register menus via 0x26ea0 / 0x26b60 with names:
//     "Debug Game"/"Toggle Debug Camera" → self+0x2688
//     "Debug Game"/"Mirror Game World"   → cell 0x2dfd1d0
//     "Debug" (0x26b60)
//     "Debug"/"Fixed TimeStep"           → self+0x2868
//     "Debug"/"DeltaTime Correction Tolorance" → cell 0x2eecde8
//   temp dtor 0xf590; menu cleanup 0x272d0; stack-chk NOT modelled
//
// Claimed: 0x255340 control flow + string/offset/cell pins.
// NOT modelled: bodies of 0x26ea0 / 0x26b60 / 0x272d0 / 0xf590 / vslot+0x50;
// stack-chk; 0x533100. PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kMid255340Rva = 0x255340;
inline constexpr std::size_t kMid255340Size = 712;
inline constexpr std::uint64_t kMid255340Cell2dfd1d0 = 0x2dfd1d0;
inline constexpr std::uint64_t kMid255340Cell2eecde8 = 0x2eecde8;
inline constexpr std::size_t kMid255340Off2688 = 0x2688;
inline constexpr std::size_t kMid255340Off2868 = 0x2868;

inline constexpr const char kMid255340NameDebugGame[] = "Debug Game";
inline constexpr const char kMid255340NameToggleCam[] = "Toggle Debug Camera";
inline constexpr const char kMid255340NameMirror[] = "Mirror Game World";
inline constexpr const char kMid255340NameDebug[] = "Debug";
inline constexpr const char kMid255340NameFixedTs[] = "Fixed TimeStep";
inline constexpr const char kMid255340NameDeltaTol[] = "DeltaTime Correction Tolorance";

struct Mid255340Hooks {
    // Models vcall [self.vt+0x50]. Null → no-op.
    void (*vslot_50)(void* self) = nullptr;
    // Models 0x26ea0 menu register. Null → no-op.
    void (*register_26ea0)(void* attach, const char* title, const char* item) = nullptr;
    // Models 0x26b60 menu register. Null → no-op.
    void (*register_26b60)(const char* title) = nullptr;
    // Models 0xf590 temp dtor. Null → no-op.
    void (*dtor_f590)(void* obj) = nullptr;
    // Models 0x272d0 menu cleanup. Null → no-op.
    void (*cleanup_272d0)(void* obj) = nullptr;
};

struct Mid255340Result {
    bool ok = false;
    int vslot_50_calls = 0;
    int register_26ea0_calls = 0;
    int register_26b60_calls = 0;
    int dtor_f590_calls = 0;
    int cleanup_272d0_calls = 0;
};

// Process-wide stand-ins for OBS attach objects at BSS RVAs (hooks receive these).
[[nodiscard]] void* mid_255340_obj_2dfd1d0() noexcept;
[[nodiscard]] void* mid_255340_obj_2eecde8() noexcept;

// Models 0x255340(rdi=self). Stack-chk NOT modelled.
[[nodiscard]] Mid255340Result mid_255340(void* self, const Mid255340Hooks& hooks = {}) noexcept;

}  // namespace sotc
