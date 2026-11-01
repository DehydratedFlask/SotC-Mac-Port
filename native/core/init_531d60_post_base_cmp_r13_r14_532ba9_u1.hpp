// Init `0x531d60` registry cmp r13,r14 OBS `0x532ba9..0x532bac` (3 B):
//   cmp r13,r14   ; epilogue loop bound after add r13,+0x30 @ `0x532ba5`
//
// After E587 add @ `0x532ba5`. NOT modelled: fall-through / loop re-entry after E589 jne @ `0x532bac`.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532ba9_U1Rva = 0x532ba9;
inline constexpr std::size_t kInit531d60PostBaseCmpR13R14_532ba9_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR13R14_532ba9_U1EndRva = 0x532bac;

struct Init531d60PostBaseCmpR13R14_532ba9_U1Cells {
    void* r13_in = nullptr;
    void* r14_in = nullptr;
};

struct Init531d60PostBaseCmpR13R14_532ba9_U1Result {
    bool ok = false;
    int cmp_r13_r14_532ba9_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR13R14_532ba9_U1Result init_531d60_post_base_cmp_r13_r14_532ba9_u1(
    const Init531d60PostBaseCmpR13R14_532ba9_U1Cells& cells = {}) noexcept;

}  // namespace sotc
