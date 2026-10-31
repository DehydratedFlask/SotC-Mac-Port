// Init `0x531d60` registry call 0x54dc20 OBS `0x532b21..0x532b26` (5 B):
//   call 0x54dc20   ; input-table count accessor (returns eax = 0x113)
//
// After E552 mov rbx,rax @ `0x532b1e`. NOT modelled: store/compare @ `0x532b33..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall54dc20_532b21_U1Rva = 0x532b21;
inline constexpr std::size_t kInit531d60PostBaseCall54dc20_532b21_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall54dc20_532b21_U1EndRva = 0x532b26;
inline constexpr std::uint64_t kInit531d60PostBaseCall54dc20_532b21_U1TargetRva = 0x54dc20;

struct Init531d60PostBaseCall54dc20_532b21_U1Cells {
    std::uint32_t* eax_out = nullptr;
};

struct Init531d60PostBaseCall54dc20_532b21_U1Result {
    bool ok = false;
    int call_54dc20_count = 0;
    std::uint32_t count = 0;
};

[[nodiscard]] Init531d60PostBaseCall54dc20_532b21_U1Result init_531d60_post_base_call_54dc20_532b21_u1(
    const Init531d60PostBaseCall54dc20_532b21_U1Cells& cells = {}) noexcept;

}  // namespace sotc
