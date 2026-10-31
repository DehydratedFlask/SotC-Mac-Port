// Init `0x531d60` continue call-site OBS `0x532760..0x532765` (5 B):
//   call 0xa543b0   ; first unit after E511 0xa2e5d0 call-site
//   // stop before mov rax,[rsp+0x20] @ 0x532765
//
// Always-on after E511 a2e5d0_u1. Intervening prep `0x532740..0x532760` (32 B:
// lea/load/add through 0x53275c) is NOT modelled — prefer this call-site over
// prep/lea tourism and over `0xa543b0` body.
// Claimed: call-site control flow pin only; optional host cells may supply rdi/rsi
// operands for the counting hook (not measured as separate units).
// NOT modelled: lea/load/add prep; 0xa543b0 body; following mov from stack.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA543b0U1Rva = 0x532760;
inline constexpr std::size_t kInit531d60PostBaseA543b0U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA543b0U1EndRva = 0x532765;
inline constexpr std::uint64_t kInit531d60PostBaseA543b0U1TargetRva = 0xa543b0;
inline constexpr std::uint64_t kInit531d60PostBaseA543b0U1PrepRva = 0x532740;
inline constexpr std::size_t kInit531d60PostBaseA543b0U1PrepSize = 32;
inline constexpr std::size_t kInit531d60PostBaseA543b0U1ObjOff = 0xf8;
inline constexpr std::size_t kInit531d60PostBaseA543b0U1RsiAddend = 0x40;

struct Init531d60PostBaseA543b0U1Hooks {
    const void* incoming_rdi = nullptr;
    std::uintptr_t incoming_rsi = 0;
    bool incoming_rsi_known = false;
    const void* const* cell_32f79a8 = nullptr;
    const void* const* cell_32f84f8 = nullptr;
    void (*call_a543b0)(void* rdi, std::uintptr_t rsi) = nullptr;
};

struct Init531d60PostBaseA543b0U1Result {
    bool ok = false;
    int call_a543b0_count = 0;
    std::uintptr_t rsi_at_0x532760 = 0;
    bool rsi_known = false;
};

[[nodiscard]] Init531d60PostBaseA543b0U1Result init_531d60_post_base_a543b0_u1(
    const Init531d60PostBaseA543b0U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
