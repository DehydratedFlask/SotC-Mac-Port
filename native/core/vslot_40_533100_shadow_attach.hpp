// App vslot `0x533100` shadowmodels attach OBS `0x53347f..0x533582` (259 B):
//   cmp success-flag [out+8]; je fail → r14=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x5334f9: install loaded into *[f08+0] (replace + refcount)
//   optional notify if success; release temps; 0xf590 path dtor
//   // stop before shadowskels @ 0x533582
//
// Always-on after E426 load-attempt. Prefer over shadowskels (later) and over
// deepen 0x14c2f0 / 0x6a5370 (already hooked earlier). Prefer over empty-path
// migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580; shadowskels. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowAttachRva = 0x53347f;
inline constexpr std::size_t kVslot40533100ShadowAttachSize = 259;
inline constexpr std::uint64_t kVslot40ShadowAttachEndRva = 0x533582;
inline constexpr std::uint64_t kVslot40ShadowAttachSlotOff = 0;  // *[f08+0]
inline constexpr std::uint64_t kVslot40ShadowAttachFlagOff = 8;  // byte [f08+8]

struct Vslot40_533100ShadowAttachHooks {
    // Models 0x14d880() → sentinel / empty marker. Null → nullptr.
    void* (*sentinel_14d880)() = nullptr;
    // Models 0x14ccc0(obj) release. Null → no-op.
    void (*release_14ccc0)(void* obj) = nullptr;
    // Models 0x14d2d0(obj) strong release. Null → no-op.
    void (*release_14d2d0)(void* obj) = nullptr;
    // Models 0x95580(f08) notify on success path. Null → no-op.
    void (*notify_95580)(void* f08) = nullptr;
    // Models 0xf590 path dtor. Null → no-op.
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100ShadowAttachResult {
    bool ok = false;
    bool took_fail_path = false;  // measured default
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

// Models attach from success-flag through cleanup. Stops before shadowskels.
// `loaded` from E426; `success_flag` false on measured path when load did not
// set the out-slot flag.
[[nodiscard]] Vslot40_533100ShadowAttachResult vslot_40_533100_shadow_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100ShadowAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
