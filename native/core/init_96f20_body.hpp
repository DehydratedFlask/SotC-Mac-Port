// Native 0x96f20 wrapper: forward four observed arguments through 0x97a50.
//
// The original is 41 B at 0x96f20..0x96f49 (sha256
// 384d9d1f8dbb887de09c7f88fd338824d54c51fab56e9309447dc43a7f51eb8d):
//
//   push rbp; mov rsp,rbp
//   eax = edx; r9d = ecx; edx = 1; ecx = 1; r8d = eax
//   push 0; push 0; push 0; push 0
//   call 0x97a50
//   add rsp,0x20; pop rbp; ret
//
// The call is therefore 0x97a50(a0, a1, 1, 1, a2, a3, 0, 0, 0, 0). The callee is a
// 2,835 B GPU/render-object builder that has no native model. This module owns only the
// wrapper and exposes that callee as an explicit host contract; it never invents the
// callee's contents. The runtime contract allocates the 0x1c0-byte object block observed at
// 0x97af7 and reports it, so the wrapper's return and the caller's cell are observable; the
// object's internal fields are not claimed as modelled.

#pragma once

#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit96f20BodyRva = 0x96f20;
inline constexpr std::size_t kInit96f20BodySize = 41;
inline constexpr std::uint64_t kInit96f20BodyEndRva = 0x96f49;
inline constexpr std::uint64_t kInit96f20CalleeRva = 0x97a50;
inline constexpr std::uint64_t kInit96f20CalleeSize = 2835;
inline constexpr std::size_t kInit96f20CalleeAllocationBytes = 0x1c0;
inline constexpr std::uint32_t kInit96f20FixedArg2 = 1;
inline constexpr std::uint32_t kInit96f20FixedArg3 = 1;

struct Init96f20Hooks {
    // 0x97a50 is intentionally an explicit host contract. The ten arguments are the exact
    // register/stack argument layout of the wrapper call; the host must return a value (or a
    // named refusal) through InitPostCallOutcome.
    InitPostCallOutcome (*call_97a50)(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                                      std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                                      std::uint32_t a6, std::uint32_t a7, std::uint32_t a8,
                                      std::uint32_t a9) = nullptr;
};

struct Init96f20Result {
    bool ok = false;
    const char* blocked = nullptr;
    std::uint64_t return_value = 0;
    InitPostCallLog call_log{};
};

[[nodiscard]] Init96f20Result run_init_96f20_body(
    std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
    const Init96f20Hooks& hooks) noexcept;

}  // namespace sotc
