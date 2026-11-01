// Init `0x531d60` registry add r13 OBS `0x532ba5..0x532ba9` (4 B):
//   add r13,0x30   ; epilogue / je @ `0x532ba5` merge target
//
// After E586 call @ `0x532ba0`. Fall-through cmp is E588 `cmp_r13_r14_532ba9_u1`.
// NOT modelled: `jne 0x532b70` @ `0x532bac`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseAddR13_30_532ba5_U1Rva = 0x532ba5;
inline constexpr std::size_t kInit531d60PostBaseAddR13_30_532ba5_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseAddR13_30_532ba5_U1EndRva = 0x532ba9;
inline constexpr std::uint32_t kInit531d60PostBaseAddR13_30_532ba5_U1Imm = 0x30;

struct Init531d60PostBaseAddR13_30_532ba5_U1Cells {
    const void* r13_in = nullptr;
    void** r13_out = nullptr;
};

struct Init531d60PostBaseAddR13_30_532ba5_U1Result {
    bool ok = false;
    int add_r13_30_532ba5_count = 0;
    void* r13 = nullptr;
};

[[nodiscard]] Init531d60PostBaseAddR13_30_532ba5_U1Result init_531d60_post_base_add_r13_30_532ba5_u1(
    const Init531d60PostBaseAddR13_30_532ba5_U1Cells& cells = {}) noexcept;

}  // namespace sotc
