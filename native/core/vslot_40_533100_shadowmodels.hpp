// App vslot `0x533100` shadowmodels load-attempt OBS `0x533435..0x53347f` (74 B):
//   cells 0x2ee2cf8 (loader) + 0x2efc9e0 (f08 singleton)
//   path "$/Models/****/shadowmodels.crmp"
//   call 0x14c2f0(path_buf, path) → hashed path
//   vcall [loader.vt+0x18](out, loader, path_buf)
//   call 0x14ccb0(result)  // lock inc [rdi+0x30]; ret
//   // stop before the attach join @ 0x53347f
//
// Always-on after E425 gfx prelude. Prefer over deepen 0x6a5370/0x6abcc0
// (already hooked in prelude, not next sequential). Prefer over empty-path
// migrate/non-null.
//
// Claimed: load-attempt control flow + path pin + lifted 0x14ccb0.
// NOT modelled: body of 0x14c2f0 (166 B); vcall body; attach (see
// vslot_40_533100_shadow_attach.*); asset I/O. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowRva = 0x533435;
inline constexpr std::size_t kVslot40533100ShadowSize = 74;
inline constexpr std::uint64_t kVslot40ShadowEndRva = 0x53347f;
inline constexpr std::uint64_t kVslot40ShadowLoaderCellRva = 0x2ee2cf8;
inline constexpr const char kVslot40ShadowPath[] = "$/Models/****/shadowmodels.crmp";
inline constexpr std::uint64_t kVslot40ShadowRefcountOff = 0x30;

struct Vslot40_533100ShadowHooks {
    // Models 0x14c2f0(dst, path_cstr). Null → no-op (dst left zeroed).
    void (*path_14c2f0)(void* dst, const char* path) = nullptr;
    // Models vcall [loader.vt+0x18](out_slot, loader, path_buf).
    // Returns result object pointer (also written to *out_slot). Null → nullptr.
    void* (*load_vslot18)(void* out_slot, void* loader, void* path_buf) = nullptr;
};

struct Vslot40_533100ShadowResult {
    bool ok = false;
    void* loader = nullptr;
    void* f08 = nullptr;
    void* loaded = nullptr;
    int path_14c2f0_count = 0;
    int load_vslot18_count = 0;
    bool refcount_bumped = false;
    std::uint32_t refcount_after = 0;
};

// Process-wide stand-in for OBS loader cell 0x2ee2cf8.
[[nodiscard]] void*& vslot40_shadow_loader_cell_2ee2cf8() noexcept;

// Lifted OBS 0x14ccb0: lock inc dword [rdi+0x30]; ret.
void refcount_inc_30_14ccb0(void* obj) noexcept;

// Models shadowmodels load-attempt. Stops before the attach join; `[out+8]` is the GetResource `owned` byte (E595).
[[nodiscard]] Vslot40_533100ShadowResult vslot_40_533100_shadowmodels(
    const Vslot40_533100ShadowHooks& hooks = {}) noexcept;

}  // namespace sotc
