// App vslot `0x533100` shadowanims load-attempt OBS `0x5336f7..0x53375a` (99 B):
//   notify 0x95580(*0x2efc9e0 + 0x10)
//   cells 0x2ee2cf8 (loader) + 0x2efc9e0 (f08); r15 = f08+0x20 (attach slot later)
//   path "$/Models/****/shadowanims.crmp"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   call 0x14ccb0(result)  // lock inc [rdi+0x30]; ret
//   // stop before success-flag branch @ 0x53375a
//
// Always-on after E429 skels attach. Prefer over success atexit (flag-true only;
// measured fail) and over deepen 0x14c2f0/0x6a5370 (already hooked earlier).
// Prefer over empty-path migrate/non-null.
//
// Claimed: notify + load-attempt control flow + path pin + lifted 0x14ccb0.
// NOT modelled: success atexit; anims attach into f08+0x20; bodies of
// 0x14c2f0 / 0x95580 / vcall; asset I/O. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowanimsRva = 0x5336f7;
inline constexpr std::size_t kVslot40533100ShadowanimsSize = 99;
inline constexpr std::uint64_t kVslot40ShadowanimsEndRva = 0x53375a;
inline constexpr std::uint64_t kVslot40ShadowanimsLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40ShadowanimsPath[] = "$/Models/****/shadowanims.crmp";
inline constexpr std::uint64_t kVslot40ShadowanimsSlotOff = 0x20;  // *[f08+0x20] later
inline constexpr std::uint64_t kVslot40ShadowanimsNotifyOff = 0x10;  // notify arg

struct Vslot40_533100ShadowanimsHooks {
    // Models 0x95580(f08+0x10) notify before load. Null → no-op.
    void (*notify_95580)(void* slot_obj) = nullptr;
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100ShadowanimsResult {
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

[[nodiscard]] Vslot40_533100ShadowanimsResult vslot_40_533100_shadowanims(
    const Vslot40_533100ShadowanimsHooks& hooks = {}) noexcept;

}  // namespace sotc
