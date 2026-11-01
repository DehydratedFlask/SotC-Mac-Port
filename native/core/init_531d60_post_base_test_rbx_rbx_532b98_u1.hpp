// Init `0x531d60` registry test rbx,rbx OBS `0x532b98..0x532b9b` (3 B):
//   test rbx,rbx   ; ZF when mov @ `0x532b94` loaded null from [r13+0x10]
//
// After E582 mov @ `0x532b94`. Je @ `0x532b9b` is E584 unit `je_532ba5_532b9b_u1`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseTestRbxRbx_532b98_U1Rva = 0x532b98;
inline constexpr std::size_t kInit531d60PostBaseTestRbxRbx_532b98_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseTestRbxRbx_532b98_U1EndRva = 0x532b9b;

struct Init531d60PostBaseTestRbxRbx_532b98_U1Cells {
    void* rbx_in = nullptr;
};

struct Init531d60PostBaseTestRbxRbx_532b98_U1Result {
    bool ok = false;
    int test_rbx_rbx_count = 0;
    bool rbx_is_zero = false;
};

[[nodiscard]] Init531d60PostBaseTestRbxRbx_532b98_U1Result init_531d60_post_base_test_rbx_rbx_532b98_u1(
    const Init531d60PostBaseTestRbxRbx_532b98_U1Cells& cells = {}) noexcept;

}  // namespace sotc
