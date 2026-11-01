// Init `0x531d60` registry cmp r15,r11 OBS `0x532b78..0x532b7b` (3 B):
//   cmp r15,r11   ; end iterator vs walk pointer after mov r15,[r13+0x18] @ `0x532b74`
//
// After E572 mov r15 @ `0x532b74`. NOT modelled: je @ `0x532b7b..` (E574 unit). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCmpR15R11_532b78_U1Rva = 0x532b78;
inline constexpr std::size_t kInit531d60PostBaseCmpR15R11_532b78_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseCmpR15R11_532b78_U1EndRva = 0x532b7b;

struct Init531d60PostBaseCmpR15R11_532b78_U1Cells {
    void* r15_in = nullptr;
    void* r11_in = nullptr;
};

struct Init531d60PostBaseCmpR15R11_532b78_U1Result {
    bool ok = false;
    int cmp_r15_r11_count = 0;
    bool pointers_equal = false;
};

[[nodiscard]] Init531d60PostBaseCmpR15R11_532b78_U1Result init_531d60_post_base_cmp_r15_r11_532b78_u1(
    const Init531d60PostBaseCmpR15R11_532b78_U1Cells& cells = {}) noexcept;

}  // namespace sotc
