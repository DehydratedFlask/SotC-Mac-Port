// Init `0x531d60` continue call-site OBS `0x532775..0x53277a` (5 B):
//   call 0x1fc80   ; first unit after E512 0xa543b0 call-site chain tip @ 0x532765
//   // stop before test rax,rax @ 0x53277a
//
// Always-on after E512 a543b0_u1. Intervening prep `0x532765..0x532775` (16 B:
// mov/load stack slot / lea rsi / lea rdi) is NOT modelled — prefer this
// call-site over prep/lea tourism and over `0x1fc80` body beyond host hooks.
// Claimed: call-site control flow pin only.
// NOT modelled: prep; lookup body semantics beyond optional native hook.
// PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase1fc80U1Rva = 0x532775;
inline constexpr std::size_t kInit531d60PostBase1fc80U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase1fc80U1EndRva = 0x53277a;
inline constexpr std::uint64_t kInit531d60PostBase1fc80U1TargetRva = 0x1fc80;
inline constexpr std::uint64_t kInit531d60PostBase1fc80U1PrepRva = 0x532765;
inline constexpr std::size_t kInit531d60PostBase1fc80U1PrepSize = 16;
inline constexpr std::uint64_t kInit531d60PostBase1fc80U1Lea1014d69Rva = 0x1014d69;

struct Init531d60PostBase1fc80U1Hooks {
    const void* const* rsp20_slot = nullptr;
    void* incoming_rdi = nullptr;
    std::uintptr_t incoming_rsi_rva = kInit531d60PostBase1fc80U1Lea1014d69Rva;
    void (*call_1fc80)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    void* (*call_1fc80_result)(void* rdi, std::uintptr_t rsi_rva) = nullptr;
    bool use_native_lookup_1fc80 = false;
    const char* nointro_needle = nullptr;
};

struct Init531d60PostBase1fc80U1Result {
    bool ok = false;
    int call_1fc80_count = 0;
    void* rax_from_1fc80 = nullptr;
    bool rax_from_1fc80_known = false;
};

[[nodiscard]] Init531d60PostBase1fc80U1Result init_531d60_post_base_1fc80_u1(
    const Init531d60PostBase1fc80U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
