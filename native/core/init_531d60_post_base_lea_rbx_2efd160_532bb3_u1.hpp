// Init `0x531d60` registry lea rbx OBS `0x532bb3..0x532bba` (7 B):
//   lea rbx,[0x2efd160]   ; epilogue reload of global registry cell after mov r13 @ `0x532bae`
//
// After E590 mov @ `0x532bae`. NOT modelled: tail @ `0x532bba..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532bb3_U1Rva = 0x532bb3;
inline constexpr std::size_t kInit531d60PostBaseLeaRbx2efd160_532bb3_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532bb3_U1EndRva = 0x532bba;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532bb3_U1TargetRva = 0x2efd160;

struct Init531d60PostBaseLeaRbx2efd160_532bb3_U1Cells {
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseLeaRbx2efd160_532bb3_U1Result {
    bool ok = false;
    int lea_rbx_2efd160_532bb3_count = 0;
    void* rbx = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRbx2efd160_532bb3_U1Result
init_531d60_post_base_lea_rbx_2efd160_532bb3_u1(
    const Init531d60PostBaseLeaRbx2efd160_532bb3_U1Cells& cells = {}) noexcept;

}  // namespace sotc
