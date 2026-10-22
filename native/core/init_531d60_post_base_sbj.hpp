// Init `0x531d60` continue call-site OBS `0x531f93..0x531fe2` (79 B):
//   lea rbx / lea rax "Shadow Boss Jump" / stack string prep (NOT modelled)
//   call 0x26800
//   mov rdi, rbx
//   call 0xf590
//   // stop before next string setup @ 0x531fe2 ("Shadow Boss Outro Jump")
//
// Always-on after E461 first 0x26800/0xf590 pair. Prefer this string-pair
// **call-site** continue over 0x26800/0xf590 body tourism, staffroll/PLT
// bodies, and deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 / 0x140d90 /
// 0x14c2f0 / 0x6a5370.
//
// Claimed: dual call-site control flow pin only.
// NOT modelled: stack/string prep; 0x26800 / 0xf590 bodies; later pairs
// (Outro Jump…) / call 0x52eb50. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseSbjRva = 0x531f93;
inline constexpr std::size_t kInit531d60PostBaseSbjSize = 79;
inline constexpr std::uint64_t kInit531d60PostBaseSbjEndRva = 0x531fe2;
inline constexpr std::uint64_t kInit531d60PostBaseSbjCall26800Rva = 0x531fd5;
inline constexpr std::uint64_t kInit531d60PostBaseSbjCallF590Rva = 0x531fdd;
inline constexpr std::uint64_t kInit531d60PostBaseSbjStringRva = 0x1014cf7;
inline constexpr std::uint64_t kInit531d60PostBaseSbjTarget26800Rva = 0x26800;
inline constexpr std::uint64_t kInit531d60PostBaseSbjTargetF590Rva = 0xf590;

struct Init531d60PostBaseSbjHooks {
    // Models 0x26800() (string helper body NOT lifted). Null → no-op.
    void (*call_26800)() = nullptr;
    // Models 0xf590() (path/dtor helper body NOT lifted). Null → no-op.
    void (*call_f590)() = nullptr;
};

struct Init531d60PostBaseSbjResult {
    bool ok = false;
    int call_26800_count = 0;
    int call_f590_count = 0;
};

// Models OBS 0x531f93..0x531fe2 on measured Init success path.
[[nodiscard]] Init531d60PostBaseSbjResult init_531d60_post_base_sbj(
    const Init531d60PostBaseSbjHooks& hooks = {}) noexcept;

}  // namespace sotc
