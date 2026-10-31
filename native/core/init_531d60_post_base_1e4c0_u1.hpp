// Init `0x531d60` continue B call-site OBS `0x53278e..0x532793` (5 B):
//   call 0x1e4c0   ; first call after continue_a fall-through @ 0x53277f
//
// Always-on on the B fall-through path. Prep `0x53277f..0x53278e` (15 B) NOT
// modelled. NOT modelled: 0x1e4c0 body beyond hook; following mov ebx,eax.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase1e4c0U1Rva = 0x53278e;
inline constexpr std::size_t kInit531d60PostBase1e4c0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase1e4c0U1EndRva = 0x532793;
inline constexpr std::uint64_t kInit531d60PostBase1e4c0U1TargetRva = 0x1e4c0;
inline constexpr std::uint64_t kInit531d60PostBase1e4c0U1PrepRva = 0x53277f;
inline constexpr std::size_t kInit531d60PostBase1e4c0U1PrepSize = 15;

struct Init531d60PostBase1e4c0U1Hooks {
    const void* incoming_rax_standin = nullptr;
    int (*call_1e4c0)(void* rdi) = nullptr;
};

struct Init531d60PostBase1e4c0U1Result {
    bool ok = false;
    int call_1e4c0_count = 0;
    int eax_return = 0;
    bool eax_known = false;
};

[[nodiscard]] Init531d60PostBase1e4c0U1Result init_531d60_post_base_1e4c0_u1(
    const Init531d60PostBase1e4c0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
