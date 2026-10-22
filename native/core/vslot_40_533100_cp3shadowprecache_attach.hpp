// App vslot `0x533100` cp3shadowprecache attach OBS `0x533f97..0x53403c` (165 B):
//   host = self (stored @ [rbp-0x300] from rdi); slot *[self+0x3cc8]; flag [self+0x3cd0]
//   sentinel 0x14d880; install loaded (replace + refcount 0x14ccb0 / 0x14ccc0)
//   success-flag @ [rbp-0xf8]; measured default false → skip 0x95580 notify
//   always: release loaded 0x14ccc0; path dtor 0xf590
//   // stop before next (0x140d90 esi=9) @ 0x53403c
//     (see vslot_40_533100_tail_140d90.*)
//
// Always-on after E442 cp3shadowprecache load-attempt. Prefer over success atexit
// (flag-true only) and over deepen 0x14c2f0 / 0x6a5370 (already hooked).
// Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup into self+0x3cc8.
// NOT modelled: success atexit notify cluster; bodies of 0x14d880/0x14d2d0/
// 0x14ccc0/0x95580. Next: see tail_140d90.*. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3shadowprecacheAttachRva = 0x533f97;
inline constexpr std::size_t kVslot40533100Cp3shadowprecacheAttachSize = 165;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachEndRva = 0x53403c;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachSlotOff = 0x3cc8;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachFlagOff = 0x3cd0;

struct Vslot40_533100Cp3shadowprecacheAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
    void (*notify_95580)(void* slot_obj) = nullptr;
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100Cp3shadowprecacheAttachResult {
    bool ok = false;
    bool took_fail_path = false;
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100Cp3shadowprecacheAttachResult
vslot_40_533100_cp3shadowprecache_attach(
    void* self, void* loaded, bool success_flag,
    const Vslot40_533100Cp3shadowprecacheAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
