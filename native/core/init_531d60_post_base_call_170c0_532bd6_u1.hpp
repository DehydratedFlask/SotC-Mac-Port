// Init `0x531d60` registry call 0x170c0 OBS `0x532bd6..0x532bdb` (5 B rel32):
//   call 0x170c0   ; tag-2 free of [r12+0x20] on fall-through after je @ `0x532bd4`
//
// After E603 je @ `0x532bd4`. The pinned bytes are `e8 e5 44 ae ff`; E599-E602 previously
// decoded `mov rax,[rdi]` / `mov r13,[rax+8]` / `mov rax,[rdi]` / `sub eax,0x010cb932` across
// this span, which crossed the real instruction boundary. E613 re-decodes it as this call.
//
// NOT modelled: E613 mov rdi,[r12] @ `0x532bdb` and the rest of the tail @ `0x532bdf..`.
// G1 PARTIAL.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bd6_U1Rva = 0x532bd6;
inline constexpr std::size_t kInit531d60PostBaseCall170c0_532bd6_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bd6_U1EndRva = 0x532bdb;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bd6_U1TargetRva = 0x170c0;

struct Init531d60PostBaseCall170c0_532bd6_U1Cells {
    void* rdi_in = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Init531d60PostBaseCall170c0_532bd6_U1Result {
    bool ok = false;
    int call_170c0_532bd6_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall170c0_532bd6_U1Result
init_531d60_post_base_call_170c0_532bd6_u1(
    const Init531d60PostBaseCall170c0_532bd6_U1Cells& cells = {}) noexcept;

}  // namespace sotc
