// Init `0x531d60` registry call 0x170b0 OBS `0x532bf3..0x532bf8` (5 B rel32):
//   call 0x170b0   ; tag-1 free(rdi) after mov rdi,r12 @ je merge `0x532be6`
//
// After E606 mov @ `0x532bf0`. NOT modelled: E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall170b0_532bf3_U1Rva = 0x532bf3;
inline constexpr std::size_t kInit531d60PostBaseCall170b0_532bf3_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170b0_532bf3_U1EndRva = 0x532bf8;
inline constexpr std::uint64_t kInit531d60PostBaseCall170b0_532bf3_U1TargetRva = 0x170b0;

struct Init531d60PostBaseCall170b0_532bf3_U1Cells {
    void* rdi_in = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Init531d60PostBaseCall170b0_532bf3_U1Result {
    bool ok = false;
    int call_170b0_532bf3_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall170b0_532bf3_U1Result init_531d60_post_base_call_170b0_532bf3_u1(
    const Init531d60PostBaseCall170b0_532bf3_U1Cells& cells = {}) noexcept;

}  // namespace sotc
