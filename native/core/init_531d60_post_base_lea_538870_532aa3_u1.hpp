// Init `0x531d60` continue E lea OBS `0x532aa3..0x532aaa` (7 B):
//   lea rcx,[0x538870]   ; thunk 2 address pin (ret no-op)
//
// After E529 `mov_rdi_r13_532a9f_u1` end @ `0x532aa3`. NOT modelled: following
// `lea rbx,[rsp+0x490]` @ `0x532aaa` (E530+ continue_e). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLea538870_532aa3_U1Rva = 0x532aa3;
inline constexpr std::size_t kInit531d60PostBaseLea538870_532aa3_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLea538870_532aa3_U1EndRva = 0x532aaa;
inline constexpr std::uint64_t kInit531d60PostBaseLea538870_532aa3_U1TargetRva = 0x538870;

struct Init531d60PostBaseLea538870_532aa3_U1Cells {
    void** rcx_out = nullptr;
};

struct Init531d60PostBaseLea538870_532aa3_U1Result {
    bool ok = false;
    int lea_rip_count = 0;
};

[[nodiscard]] Init531d60PostBaseLea538870_532aa3_U1Result init_531d60_post_base_lea_538870_532aa3_u1(
    const Init531d60PostBaseLea538870_532aa3_U1Cells& cells = {}) noexcept;

}  // namespace sotc
