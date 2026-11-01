// Init `0x531d60` registry call 0x170c0 OBS `0x532ba0..0x532ba5` (5 B rel32):
//   call 0x170c0   ; tag-2 free(rdi)
//
// After E585 mov @ `0x532b9d`. NOT modelled: epilogue @ `0x532ba5`. G1 PARTIAL.
#pragma once

#include "alloc_wrappers_17070.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532ba0_U1Rva = 0x532ba0;
inline constexpr std::size_t kInit531d60PostBaseCall170c0_532ba0_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532ba0_U1EndRva = 0x532ba5;
inline constexpr std::uint64_t kInit531d60PostBaseCall170c0_532ba0_U1TargetRva = 0x170c0;

struct Init531d60PostBaseCall170c0_532ba0_U1Cells {
    void* rdi_in = nullptr;
    const AllocDispatchContract* alloc = nullptr;
};

struct Init531d60PostBaseCall170c0_532ba0_U1Result {
    bool ok = false;
    int call_170c0_count = 0;
};

[[nodiscard]] Init531d60PostBaseCall170c0_532ba0_U1Result init_531d60_post_base_call_170c0_532ba0_u1(
    const Init531d60PostBaseCall170c0_532ba0_U1Cells& cells = {}) noexcept;

}  // namespace sotc
