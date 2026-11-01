// Init `0x531d60` registry test r13,r13 OBS `0x532bbf..0x532bc2` (3 B):
//   test r13,r13  ; epilogue after mov r14,[rsp+0x10] @ `0x532bba` (`4d 85 ed`)
//
// After E592 mov @ `0x532bba`. NOT modelled: E594 je @ `0x532bc2` / E595 mov @ `0x532bc4` / tail @ `0x532bc7..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532bbf_U1Rva = 0x532bbf;
inline constexpr std::size_t kInit531d60PostBaseCmpR13R14_532bbf_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532bbf_U1EndRva = 0x532bc2;

struct Init531d60PostBaseCmpR13R14_532bbf_U1Cells {
    void* r13_in = nullptr;
    void* r14_in = nullptr;
};

struct Init531d60PostBaseCmpR13R14_532bbf_U1Result {
    bool ok = false;
    int cmp_r13_r14_532bbf_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR13R14_532bbf_U1Result init_531d60_post_base_cmp_r13_r14_532bbf_u1(
    const Init531d60PostBaseCmpR13R14_532bbf_U1Cells& cells = {}) noexcept;

}  // namespace sotc
