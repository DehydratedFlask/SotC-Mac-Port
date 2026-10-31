// Init `0x531d60` registry mov [rsp+0x10],r14 OBS `0x532b56..0x532b5b` (5 B):
//   mov [rsp+0x10],r14   ; destroy-path prologue after je fall-through @ `0x532b50`
//
// After E564 je @ `0x532b50`. NOT modelled: reload/compare/destroy @ `0x532b5b..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp10_532b56_U1Rva = 0x532b56;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRsp10_532b56_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp10_532b56_U1EndRva = 0x532b5b;

struct Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells {
    const void* r14_in = nullptr;
    void** rsp10_out = nullptr;
};

struct Init531d60PostBaseMovR14MemRsp10_532b56_U1Result {
    bool ok = false;
    int mov_r14_mem_rsp10_count = 0;
    void* r14_stored = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRsp10_532b56_U1Result
init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1(
    const Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells& cells = {}) noexcept;

}  // namespace sotc
