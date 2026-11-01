// Init `0x531d60` registry call 0x170c0 OBS `0x532bc7..0x532bcc` (5 B rel32):
//   call 0x170c0   ; tag-2 free(rdi) on fall-through after mov r13,r15 @ `0x532bc4`
//
// After E595 mov @ `0x532bc4`. NOT modelled: E597 mov @ `0x532bcc` / tail @ `0x532bd1..`. G1 PARTIAL.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bc7_U1Rva = 0x532bc7;
inline constexpr std::size_t kInit531d60PostBaseCall170c0_532bc7_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bc7_U1EndRva = 0x532bcc;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532bc7_U1TargetRva = 0x170c0;

struct Init531d60PostBaseCall170c0_532bc7_U1Cells {
    void* rdi_in = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Init531d60PostBaseCall170c0_532bc7_U1Result {
    bool ok = false;
    int call_170c0_532bc7_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall170c0_532bc7_U1Result init_531d60_post_base_call_170c0_532bc7_u1(
    const Init531d60PostBaseCall170c0_532bc7_U1Cells& cells = {}) noexcept;

}  // namespace sotc
