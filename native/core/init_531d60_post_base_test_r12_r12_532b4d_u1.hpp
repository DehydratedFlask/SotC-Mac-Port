// Init `0x531d60` registry test r12,r12 OBS `0x532b4d..0x532b50` (3 B):
//   test r12,r12   ; ZF when previous cell @ *(0x2efd160) is null
//
// After E562 je fall-through @ `0x532b47`. NOT modelled: je @ `0x532b50` or destroy/store @
// `0x532b50..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseTestR12R12_532b4d_U1Rva = 0x532b4d;
inline constexpr std::size_t kInit531d60PostBaseTestR12R12_532b4d_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseTestR12R12_532b4d_U1EndRva = 0x532b50;

struct Init531d60PostBaseTestR12R12_532b4d_U1Cells {
    void* r12_in = nullptr;
};

struct Init531d60PostBaseTestR12R12_532b4d_U1Result {
    bool ok = false;
    int test_r12_r12_count = 0;
    bool r12_is_zero = false;
};

[[nodiscard]] Init531d60PostBaseTestR12R12_532b4d_U1Result init_531d60_post_base_test_r12_r12_532b4d_u1(
    const Init531d60PostBaseTestR12R12_532b4d_U1Cells& cells = {}) noexcept;

}  // namespace sotc
