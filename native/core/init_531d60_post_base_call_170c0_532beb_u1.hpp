// Init `0x531d60` registry call 0x170c0 OBS `0x532beb..0x532bf0` (5 B rel32):
//   call 0x170c0   ; tag-2 free(rdi) on fall-through after je @ `0x532be6`
//
// After E604 je @ `0x532be6`. NOT modelled: tail @ `0x532bf0`.. G1 PARTIAL.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532beb_U1Rva = 0x532beb;
inline constexpr std::size_t kInit531d60PostBaseCall170c0_532beb_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532beb_U1EndRva = 0x532bf0;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532beb_U1TargetRva = 0x170c0;

struct Init531d60PostBaseCall170c0_532beb_U1Cells {
    void* rdi_in = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Init531d60PostBaseCall170c0_532beb_U1Result {
    bool ok = false;
    int call_170c0_532beb_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall170c0_532beb_U1Result init_531d60_post_base_call_170c0_532beb_u1(
    const Init531d60PostBaseCall170c0_532beb_U1Cells& cells = {}) noexcept;

}  // namespace sotc
