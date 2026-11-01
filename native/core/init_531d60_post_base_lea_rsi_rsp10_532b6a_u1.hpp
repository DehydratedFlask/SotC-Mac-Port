// Init `0x531d60` registry lea rsi OBS `0x532b6a..0x532b6f` (5 B):
//   lea rsi,[rsp+0x10]   ; fall-through after je @ `0x532b68`
//
// After E569 je @ `0x532b68`. NOT modelled: destroy/load tail @ `0x532b70..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRsiRsp10_532b6a_U1Rva = 0x532b6a;
inline constexpr std::size_t kInit531d60PostBaseLeaRsiRsp10_532b6a_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRsiRsp10_532b6a_U1EndRva = 0x532b6f;
inline constexpr std::uint32_t kInit531d60PostBaseLeaRsiRsp10_532b6a_U1Disp = 0x10;

struct Init531d60PostBaseLeaRsiRsp10_532b6a_U1Cells {
    const void* stack_rsp = nullptr;
    void** rsi_out = nullptr;
};

struct Init531d60PostBaseLeaRsiRsp10_532b6a_U1Result {
    bool ok = false;
    int lea_rsi_rsp10_count = 0;
    void* rsi = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRsiRsp10_532b6a_U1Result
init_531d60_post_base_lea_rsi_rsp10_532b6a_u1(
    const Init531d60PostBaseLeaRsiRsp10_532b6a_U1Cells& cells = {}) noexcept;

}  // namespace sotc
