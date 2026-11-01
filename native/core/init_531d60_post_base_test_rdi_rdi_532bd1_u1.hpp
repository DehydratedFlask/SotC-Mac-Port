// Init `0x531d60` registry test rdi,rdi OBS `0x532bd1..0x532bd4` (3 B):
//   test rdi,rdi   ; ZF when mov @ `0x532bcc` loaded null from [r12+0x20]
//
// After E597 mov @ `0x532bcc`. NOT modelled: je @ `0x532bd4`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseTestRdiRdi_532bd1_U1Rva = 0x532bd1;
inline constexpr std::size_t kInit531d60PostBaseTestRdiRdi_532bd1_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseTestRdiRdi_532bd1_U1EndRva = 0x532bd4;

struct Init531d60PostBaseTestRdiRdi_532bd1_U1Cells {
    void* rdi_in = nullptr;
};

struct Init531d60PostBaseTestRdiRdi_532bd1_U1Result {
    bool ok = false;
    int test_rdi_rdi_count = 0;
    bool rdi_is_zero = false;
};

[[nodiscard]] Init531d60PostBaseTestRdiRdi_532bd1_U1Result init_531d60_post_base_test_rdi_rdi_532bd1_u1(
    const Init531d60PostBaseTestRdiRdi_532bd1_U1Cells& cells = {}) noexcept;

}  // namespace sotc
