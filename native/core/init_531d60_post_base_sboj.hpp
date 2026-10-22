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
    // Models 0xf590() (path/dtor helper body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseSbojResult {
    bool ok = false;
    int call_26800_count = 0;
    int call_f590_count = 0;
};

// Models OBS 0x531fe2..0x532031 on measured Init success path.
[[nodiscard]] Init531d60PostBaseSbojResult init_531d60_post_base_sboj(
    const Init531d60PostBaseSbojHooks& hooks = {}) noexcept;

}  // namespace sotc
