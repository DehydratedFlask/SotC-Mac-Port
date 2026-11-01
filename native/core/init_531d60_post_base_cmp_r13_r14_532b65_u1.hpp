// Init `0x531d60` registry cmp r13,r14 OBS `0x532b65..0x532b68` (3 B):
//   cmp r13,r14   ; destroy-path stack slots after mov r13,[rsp+0x40] / mov r14,[rsp+0x48]
//
// After E567 mov r14,[rsp+0x48] @ `0x532b60`. NOT modelled: je/destroy @ `0x532b68..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532b65_U1Rva = 0x532b65;
inline constexpr std::size_t kInit531d60PostBaseCmpR13R14_532b65_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532b65_U1EndRva = 0x532b68;

struct Init531d60PostBaseCmpR13R14_532b65_U1Cells {
    void* r13_in = nullptr;
    void* r14_in = nullptr;
};

struct Init531d60PostBaseCmpR13R14_532b65_U1Result {
    bool ok = false;
    int cmp_r13_r14_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR13R14_532b65_U1Result init_531d60_post_base_cmp_r13_r14_532b65_u1(
    const Init531d60PostBaseCmpR13R14_532b65_U1Cells& cells = {}) noexcept;

}  // namespace sotc
