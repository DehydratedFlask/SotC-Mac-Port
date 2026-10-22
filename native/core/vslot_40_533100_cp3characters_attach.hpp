// App vslot `0x533100` cp3Characters attach OBS `0x533eac..0x533f51` (165 B):
//   host = self (stored @ [rbp-0x300] from rdi); slot *[self+0x3cb8]; flag [self+0x3cc0]
//   sentinel 0x14d880; install loaded (replace + refcount 0x14ccb0 / 0x14ccc0)
//   success-flag @ [rbp-0xe8]; measured default false → skip 0x95580 notify
//   always: release loaded 0x14ccc0; path dtor 0xf590
//   // stop before next path (cp3shadowprecache) @ 0x533f51
//     (see vslot_40_533100_cp3shadowprecache.*)
//
// Always-on after E440 cp3Characters load-attempt. Prefer over success atexit
// (flag-true only) and over deepen 0x14c2f0 / 0x6a5370 (already hooked).
// Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup into self+0x3cb8.
// NOT modelled: success atexit notify cluster; bodies of 0x14d880/0x14d2d0/
// 0x14ccc0/0x95580. Next path: see cp3shadowprecache.*. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3charactersAttachRva = 0x533eac;
inline constexpr std::size_t kVslot40533100Cp3charactersAttachSize = 165;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachEndRva = 0x533f51;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachSlotOff = 0x3cb8;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachFlagOff = 0x3cc0;

struct Vslot40_533100Cp3charactersAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
    void (*notify_95580)(void* slot_obj) = nullptr;
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100Cp3charactersAttachResult {
    bool ok = false;
    bool took_fail_path = false;
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100Cp3charactersAttachResult vslot_40_533100_cp3characters_attach(
    void* self, void* loaded, bool success_flag,
    const Vslot40_533100Cp3charactersAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
