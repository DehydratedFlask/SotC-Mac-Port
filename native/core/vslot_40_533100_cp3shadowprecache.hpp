// App vslot `0x533100` cp3shadowprecache load-attempt OBS `0x533f51..0x533f97` (70 B):
//   notify 0x95580(self+0x3cb8)   // after cp3Characters slot install
//   cells 0x2ee2cf8 (loader); self from [rbp-0x300]
//   path "$/cp3shadow/****/cp3shadowprecache.calt"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   // stop before attach-into self+0x3cc8 @ 0x533f97 (14d880 join)
//
// Always-on after E441 cp3Characters attach. Prefer over success atexit
// (flag-true only) and over deepen 0x14c2f0/0x6a5370 (already hooked earlier).
// Prefer over empty-path migrate/non-null.
//
// Claimed: notify + load-attempt control flow + path pin.
// NOT modelled: attach/install into self+0x3cc8; 0x14ccb0 (in attach);
// success atexit; bodies of 0x14c2f0 / 0x95580 / vcall; asset I/O.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3shadowprecacheRva = 0x533f51;
inline constexpr std::size_t kVslot40533100Cp3shadowprecacheSize = 70;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheEndRva = 0x533f97;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40Cp3shadowprecachePath[] =
    "$/cp3shadow/****/cp3shadowprecache.calt";
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheNotifyOff = 0x3cb8;  // notify arg

struct Vslot40_533100Cp3shadowprecacheHooks {
    // Models 0x95580(self+0x3cb8) notify before load. Null → no-op.
    void (*notify_95580)(void* slot_obj) = nullptr;
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100Cp3shadowprecacheResult {
    bool ok = false;
    void* loader = nullptr;
    void* self = nullptr;
    void* loaded = nullptr;
    int notify_95580_count = 0;
    int path_14c2f0_count = 0;
    int load_vslot18_count = 0;
};

[[nodiscard]] Vslot40_533100Cp3shadowprecacheResult vslot_40_533100_cp3shadowprecache(
    void* self, const Vslot40_533100Cp3shadowprecacheHooks& hooks = {}) noexcept;

}  // namespace sotc
