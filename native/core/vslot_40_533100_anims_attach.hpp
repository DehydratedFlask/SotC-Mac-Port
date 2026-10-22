// App vslot `0x533100` shadowanims attach OBS `0x53375a..0x533870` (278 B):
//   cmp success-flag [out+8]; je fail → r12=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x5337d7: install loaded into *[f08+0x20] (replace + refcount)
//   flag byte at f08+0x28; optional notify if success; release temps; 0xf590
//   // stop before shadowbanks @ 0x533870 (see vslot_40_533100_shadowbanks.*)
//
// Always-on after E430 shadowanims load-attempt. Prefer over success atexit
// (flag-true only) and over deepen 0x14c2f0 / 0x6a5370 (already hooked).
// Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow into f08+0x20.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580; next path past end. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100AnimsAttachRva = 0x53375a;
inline constexpr std::size_t kVslot40533100AnimsAttachSize = 278;
inline constexpr std::uint64_t kVslot40AnimsAttachEndRva = 0x533870;
inline constexpr std::uint64_t kVslot40AnimsAttachSlotOff = 0x20;  // *[f08+0x20]
inline constexpr std::uint64_t kVslot40AnimsAttachFlagOff = 0x28;  // byte [f08+0x28]

struct Vslot40_533100AnimsAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
    void (*notify_95580)(void* slot_obj) = nullptr;
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100AnimsAttachResult {
    bool ok = false;
    bool took_fail_path = false;
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100AnimsAttachResult vslot_40_533100_anims_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100AnimsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
