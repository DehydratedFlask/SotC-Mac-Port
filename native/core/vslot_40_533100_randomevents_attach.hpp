// App vslot `0x533100` shadowrandomevents attach OBS `0x533bc5..0x533cdb` (278 B):
//   cmp success-flag; je fail → r12=0  (measured default)
//   // success path (atexit/tag) NOT modelled when flag false
//   join @ 0x533c3f: install loaded into *[f08+0x60] (replace + refcount)
//   flag byte at f08+0x68; optional notify if success; release temps; 0xf590
//   // stop before next load @ 0x533cdb
//
// Always-on after E436 shadowrandomevents load-attempt. Prefer over success
// atexit (flag-true only) and over deepen 0x14c2f0 / 0x6a5370 (already hooked).
// Prefer over empty-path migrate/non-null.
//
// Claimed: fail-path + always-on install/cleanup control flow into f08+0x60.
// NOT modelled: success atexit cluster (0xf8f3d0/0xf730/0x14c4a0/…);
// bodies of 0x14d880/0x14d2d0/0x14ccc0/0x95580; next path. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100RandomeventsAttachRva = 0x533bc5;
inline constexpr std::size_t kVslot40533100RandomeventsAttachSize = 278;
inline constexpr std::uint64_t kVslot40RandomeventsAttachEndRva = 0x533cdb;
inline constexpr std::uint64_t kVslot40RandomeventsAttachSlotOff = 0x60;  // *[f08+0x60]
inline constexpr std::uint64_t kVslot40RandomeventsAttachFlagOff = 0x68;  // byte [f08+0x68]

struct Vslot40_533100RandomeventsAttachHooks {
    void* (*sentinel_14d880)() = nullptr;
    void (*release_14ccc0)(void* obj) = nullptr;
    void (*release_14d2d0)(void* obj) = nullptr;
    void (*notify_95580)(void* slot_obj) = nullptr;
    void (*dtor_f590)(void* obj) = nullptr;
};

struct Vslot40_533100RandomeventsAttachResult {
    bool ok = false;
    bool took_fail_path = false;
    bool installed = false;
    void* prev_slot = nullptr;
    void* slot_after = nullptr;
    int release_14ccc0_count = 0;
    int dtor_f590_count = 0;
};

[[nodiscard]] Vslot40_533100RandomeventsAttachResult vslot_40_533100_randomevents_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100RandomeventsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
