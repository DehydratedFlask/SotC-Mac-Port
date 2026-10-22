// App vslot `0x533100` shadowbanks attach OBS `0x5338d3..0x5339e9` (278 B):
//   cmp success-flag [out+8]; je fail → r12=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x53394d: install loaded into *[f08+0x40] (replace + refcount)
//   flag byte at f08+0x48; optional notify if success; release temps; 0xf590
//   // stop before shadowstreams @ 0x5339e9 (see vslot_40_533100_shadowstreams.*)
//
// Always-on after E432 shadowbanks load-attempt. Prefer over success atexit
// (flag-true only), over jump to shadowstreams, and over deepen 0x14c2f0 /
// 0x6a5370 (already hooked). Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow into f08+0x40.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580; shadowstreams. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100BanksAttachRva = 0x5338d3;
inline constexpr std::size_t kVslot40533100BanksAttachSize = 278;
inline constexpr std::uint64_t kVslot40BanksAttachEndRva = 0x5339e9;
inline constexpr std::uint64_t kVslot40BanksAttachSlotOff = 0x40;  // *[f08+0x40]
inline constexpr std::uint64_t kVslot40BanksAttachFlagOff = 0x48;  // byte [f08+0x48]

struct Vslot40_533100BanksAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
    void (*notify_95580)(void* slot_obj) = nullptr;
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100BanksAttachResult {
    bool ok = false;
    bool took_fail_path = false;
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100BanksAttachResult vslot_40_533100_banks_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100BanksAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
