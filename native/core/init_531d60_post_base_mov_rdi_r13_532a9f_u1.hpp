// Init `0x531d60` continue E mov OBS `0x532a9f..0x532aa3` (4 B):
//   mov rdi,[r13]   ; save-data ctor 3 parent arg
//
// After E528 `movq_rsp4a0_0_532a93_u1` end @ `0x532a9f`. NOT modelled: following
// `lea rcx,[0x538870]` @ `0x532aa3`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiR13_532a9f_U1Rva = 0x532a9f;
inline constexpr std::size_t kInit531d60PostBaseMovRdiR13_532a9f_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiR13_532a9f_U1EndRva = 0x532aa3;

struct Init531d60PostBaseMovRdiR13_532a9f_U1Cells {
    // Host stand-in for r13 (parent pointer passed to ctor 3). Null → counted, rdi=null.
    const void* incoming_r13 = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiR13_532a9f_U1Result {
    bool ok = false;
    int mov_rdi_r13_count = 0;
    void* parent = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiR13_532a9f_U1Result init_531d60_post_base_mov_rdi_r13_532a9f_u1(
    const Init531d60PostBaseMovRdiR13_532a9f_U1Cells& cells = {}) noexcept;

}  // namespace sotc
