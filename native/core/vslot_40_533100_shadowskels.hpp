// App vslot `0x533100` shadowskels load-attempt OBS `0x533582..0x5335e1` (95 B):
//   notify 0x95580(*0x2efc9e0)
//   cells 0x2ee2cf8 (loader) + 0x2efc9e0 (f08); r15 = f08+0x10 (attach slot later)
//   path "$/Models/****/shadowskels.crmp"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   call 0x14ccb0(result)  // lock inc [rdi+0x30]; ret
//   // stop before success-flag branch @ 0x5335e1
//
// Always-on after E427 shadowmodels attach. Prefer over success atexit
// (only on flag-true path; measured fail) and over deepen 0x14c2f0/0x6a5370
// (already hooked earlier). Prefer over empty-path migrate/non-null.
//
// Claimed: notify + load-attempt control flow + path pin + lifted 0x14ccb0.
// NOT modelled: success atexit cluster; skels attach into f08+0x10; bodies of
// 0x14c2f0 / 0x95580 / vcall; asset I/O. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowskelsRva = 0x533582;
inline constexpr std::size_t kVslot40533100ShadowskelsSize = 95;
inline constexpr std::uint64_t kVslot40ShadowskelsEndRva = 0x5335e1;
inline constexpr std::uint64_t kVslot40ShadowskelsLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40ShadowskelsPath[] = "$/Models/****/shadowskels.crmp";
inline constexpr std::uint64_t kVslot40ShadowskelsSlotOff = 0x10;  // *[f08+0x10] later

struct Vslot40_533100ShadowskelsHooks {
    // Models 0x95580(f08) notify before load. Null → no-op.
    void (*notify_95580)(void* f08) = nullptr;
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    // Returns result object pointer (also written to *out_slot). Null → nullptr.
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100ShadowskelsResult {
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

// Models shadowskels load-attempt. Stops before success-flag branch.
[[nodiscard]] Vslot40_533100ShadowskelsResult vslot_40_533100_shadowskels(
    const Vslot40_533100ShadowskelsHooks& hooks = {}) noexcept;

}  // namespace sotc
