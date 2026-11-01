// Init `0x531d60` registry lea rax OBS `0x532bfb..0x532c02` (7 B):
//   lea rax,[0x2ee2cf8]   ; ResourcePool loader cell after mov [rbx],r14 @ `0x532bf8`
//
// After E608 mov @ `0x532bf8`. NOT modelled: tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1Rva = 0x532bfb;
inline constexpr std::size_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva = 0x532c02;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1TargetRva = 0x2ee2cf8;

struct Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells {
    void** rax_out = nullptr;
};

struct Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result {
    bool ok = false;
    int lea_rax_2ee2cf8_532bfb_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result
init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1(
    const Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells& cells = {}) noexcept;

}  // namespace sotc
