// App vslot `0x533100` cp3Characters load-attempt OBS `0x533e54..0x533eac` (88 B):
//   notify 0x95580(*0x2efc9e0 + 0x70)   // after bossmusic slot install
//   cells 0x2ee2cf8 (loader) + 0x2efc9e0 (f08)
//   path "$/characters/shadow/****/cp3Characters.calt"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   // stop before attach-into host+0x3cb8 @ 0x533eac (14d880 join)
//
// Always-on after E439 bossmusic attach. Prefer over success atexit (flag-true
// only at 0x533f01) and over deepen 0x14c2f0/0x6a5370 (already hooked earlier).
// Prefer over empty-path migrate/non-null.
//
// Claimed: notify + load-attempt control flow + path pin.
// NOT modelled: attach/install into host+0x3cb8; 0x14ccb0 (occurs in attach);
// success atexit; bodies of 0x14c2f0 / 0x95580 / vcall; asset I/O.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3charactersRva = 0x533e54;
inline constexpr std::size_t kVslot40533100Cp3charactersSize = 88;
inline constexpr std::uint64_t kVslot40Cp3charactersEndRva = 0x533eac;
inline constexpr std::uint64_t kVslot40Cp3charactersLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40Cp3charactersPath[] =
    "$/characters/shadow/****/cp3Characters.calt";
inline constexpr std::uint64_t kVslot40Cp3charactersNotifyOff = 0x70;  // notify arg

struct Vslot40_533100Cp3charactersHooks {
    // Models 0x95580(f08+0x70) notify before load. Null → no-op.
    void (*notify_95580)(void* slot_obj) = nullptr;
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100Cp3charactersResult {
    bool ok = false;
    void* loader = nullptr;
    void* f08 = nullptr;
    void* loaded = nullptr;
    int notify_95580_count = 0;
    int path_14c2f0_count = 0;
    int load_vslot18_count = 0;
};

[[nodiscard]] Vslot40_533100Cp3charactersResult vslot_40_533100_cp3characters(
    const Vslot40_533100Cp3charactersHooks& hooks = {}) noexcept;

}  // namespace sotc
