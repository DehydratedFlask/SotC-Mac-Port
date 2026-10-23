// Init `0x531d60` continue call-site OBS `0x531fe2..0x532031` (79 B):
//   lea rbx / lea rax "Shadow Boss Outro Jump" / stack string prep (NOT modelled)
//   call 0x26800
//   mov rdi, rbx
//   call 0xf590
//   // stop before call 0x52eb50 @ 0x532031
//
// Always-on after E462 Shadow Boss Jump pair. Prefer this string-pair
// **call-site** continue over 0x26800/0xf590 / 0x52eb50 / staffroll/PLT body
// tourism and deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370.
//
// Claimed: dual call-site control flow pin only.
// NOT modelled: stack/string prep; 0x26800 / 0xf590 bodies; call 0x52eb50.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseSbojRva = 0x531fe2;
inline constexpr std::size_t kInit531d60PostBaseSbojSize = 79;
inline constexpr std::uint64_t kInit531d60PostBaseSbojEndRva = 0x532031;
inline constexpr std::uint64_t kInit531d60PostBaseSbojCall26800Rva = 0x532024;
inline constexpr std::uint64_t kInit531d60PostBaseSbojCallF590Rva = 0x53202c;
inline constexpr std::uint64_t kInit531d60PostBaseSbojStringRva = 0x1014d08;
inline constexpr std::uint64_t kInit531d60PostBaseSbojTarget26800Rva = 0x26800;
inline constexpr std::uint64_t kInit531d60PostBaseSbojTargetF590Rva = 0xf590;

struct Init531d60PostBaseSbojHooks {
    // Models 0x26800() (string helper body NOT lifted). Null → no-op.
    void (*call_26800)() = nullptr;
    // 0xf590(rdi = &holder) at 0x53202c. Original: 32 B stack temp at rsp+0x1f8, `+0x10..0x1f`
    // zeroed before the call, so the release finds no control block and does nothing.
    // Null => run the lifted release; non-null => host override, whose result is reported instead.
    SharedReleaseF590Result (*call_f590)(SharedHolderF590&) = nullptr;
    // 0x12af0 destroy / 0x170b0 free-control contracts; only reached with a non-null `+0x18`.
    SharedReleaseF590Hooks release_hooks{};
};

struct Init531d60PostBaseSbojResult {
    bool ok = false;
    int call_26800_count = 0;
    int call_f590_count = 0;
    // Measured 0xf590 effect: the holder's `+0x18` is zero, so nothing is destroyed or freed.
    SharedReleaseF590Result release{};
};

// Models OBS 0x531fe2..0x532031 on measured Init success path.
[[nodiscard]] Init531d60PostBaseSbojResult init_531d60_post_base_sboj(
    const Init531d60PostBaseSbojHooks& hooks = {}) noexcept;

}  // namespace sotc
