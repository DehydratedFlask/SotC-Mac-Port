// Init `0x531d60` continue call-site OBS `0x53273b..0x532740` (5 B):
//   call 0xa2e5d0   ; first unit after E510 0xa2ea00 call-site
//   // stop before lea rax,[0x32f79a8] @ 0x532740
//
// Always-on after E510 a2ea00_u1. Intervening prep `0x532736..0x53273b` (5 B:
// mov rdi,rbx / mov esi,eax) is NOT modelled — prefer this call-site over prep
// tourism and over `0xa2e5d0` body. Prefer over deepen `0x1290` / `0x5c64c0` /
// `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` / `0x6a5370`.
// Claimed: call-site control flow pin only.
// NOT modelled: mov rdi,rbx / mov esi,eax prep; 0xa2e5d0 body; following lea.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA2e5d0U1Rva = 0x53273b;
inline constexpr std::size_t kInit531d60PostBaseA2e5d0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA2e5d0U1EndRva = 0x532740;
inline constexpr std::uint64_t kInit531d60PostBaseA2e5d0U1TargetRva = 0xa2e5d0;
inline constexpr std::uint64_t kInit531d60PostBaseA2e5d0U1PrepRva = 0x532736;
inline constexpr std::size_t kInit531d60PostBaseA2e5d0U1PrepSize = 5;

struct Init531d60PostBaseA2e5d0U1Hooks {
    const void* incoming_rbx = nullptr;
    int incoming_esi = 0;
    void (*call_a2e5d0)(void* rbx, int esi) = nullptr;
};

struct Init531d60PostBaseA2e5d0U1Result {
    bool ok = false;
    int call_a2e5d0_count = 0;
};

[[nodiscard]] Init531d60PostBaseA2e5d0U1Result init_531d60_post_base_a2e5d0_u1(
    const Init531d60PostBaseA2e5d0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
