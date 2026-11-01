// Init `0x531d60` registry mov r14,[rsp+0x10] OBS `0x532bba..0x532bbf` (5 B):
//   mov r14,[rsp+0x10]   ; fall-through after lea @ `0x532bb3`
//
// After E591 lea @ `0x532bb3`. NOT modelled: tail @ `0x532bbf..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp10_532bba_U1Rva = 0x532bba;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRsp10_532bba_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp10_532bba_U1EndRva = 0x532bbf;
inline constexpr std::uint32_t kInit531d60PostBaseMovR14MemRsp10_532bba_U1Disp = 0x10;

struct Init531d60PostBaseMovR14MemRsp10_532bba_U1Cells {
    void** rsp10_in = nullptr;
    void** r14_out = nullptr;
};

struct Init531d60PostBaseMovR14MemRsp10_532bba_U1Result {
    bool ok = false;
    int mov_r14_mem_rsp10_532bba_count = 0;
    void* r14 = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRsp10_532bba_U1Result
init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1(
    const Init531d60PostBaseMovR14MemRsp10_532bba_U1Cells& cells = {}) noexcept;

}  // namespace sotc
