// Init `0x531d60` registry lea rbx OBS `0x532b33..0x532b3a` (7 B):
//   lea rbx,[0x2efd160]   ; address of global registry cell
//
// After E557 call 0x52a220 @ `0x532b2e`. NOT modelled: mov r13 @ `0x532b3a` (E559 peel) / compare-store @ `0x532b41..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532b33_U1Rva = 0x532b33;
inline constexpr std::size_t kInit531d60PostBaseLeaRbx2efd160_532b33_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532b33_U1EndRva = 0x532b3a;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRbx2efd160_532b33_U1TargetRva = 0x2efd160;

struct Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells {
    void** rbx_out = nullptr;
};

struct Init531d60PostBaseLeaRbx2efd160_532b33_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
    void* rbx = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRbx2efd160_532b33_U1Result
init_531d60_post_base_lea_rbx_2efd160_532b33_u1(
    const Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells& cells = {}) noexcept;

}  // namespace sotc
