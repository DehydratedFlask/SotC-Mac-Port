// App vslot `0x533100` shadowstreams load-attempt OBS `0x5339e9..0x533a4c` (99 B):
//   notify 0x95580(*0x2efc9e0 + 0x40)
//   cells 0x2ee2cf8 (loader) + 0x2efc9e0 (f08); r15 = f08+0x50 (attach later)
//   path "$/sound/shadow/****/shadowstreams.crmp"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   call 0x14ccb0(result)  // lock inc [rdi+0x30]; ret
//   // stop before success-flag branch @ 0x533a4c
//
// Always-on after E433 banks attach. Prefer over success atexit (flag-true only;
// measured fail) and over deepen 0x14c2f0/0x6a5370 (already hooked earlier).
// Prefer over empty-path migrate/non-null.
//
// Claimed: notify + load-attempt control flow + path pin + lifted 0x14ccb0.
// NOT modelled: success atexit; attach/install; bodies of 0x14c2f0 / 0x95580 /
// vcall; asset I/O. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowstreamsRva = 0x5339e9;
inline constexpr std::size_t kVslot40533100ShadowstreamsSize = 99;
inline constexpr std::uint64_t kVslot40ShadowstreamsEndRva = 0x533a4c;
inline constexpr std::uint64_t kVslot40ShadowstreamsLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40ShadowstreamsPath[] =
    "$/sound/shadow/****/shadowstreams.crmp";
inline constexpr std::uint64_t kVslot40ShadowstreamsNotifyOff = 0x40;  // notify arg

struct Vslot40_533100ShadowstreamsHooks {
    // Models 0x95580(f08+0x40) notify before load. Null → no-op.
    void (*notify_95580)(void* slot_obj) = nullptr;
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100ShadowstreamsResult {
    bool ok = false;
    void* loader = nullptr;
    void* f08 = nullptr;
    void* loaded = nullptr;
    int notify_95580_count = 0;
    int path_14c2f0_count = 0;
    int load_vslot18_count = 0;
    bool refcount_bumped = false;
    std::uint32_t refcount_after = 0;
};

[[nodiscard]] Vslot40_533100ShadowstreamsResult vslot_40_533100_shadowstreams(
    const Vslot40_533100ShadowstreamsHooks& hooks = {}) noexcept;

}  // namespace sotc
