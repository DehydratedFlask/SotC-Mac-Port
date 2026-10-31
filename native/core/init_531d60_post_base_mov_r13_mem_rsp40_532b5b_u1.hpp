// Init `0x531d60` registry mov r13,[rsp+0x40] OBS `0x532b5b..0x532b60` (5 B):
//   mov r13,[rsp+0x40]   ; destroy-path prologue after mov [rsp+0x10],r14 @ `0x532b56`
//
// After E565 mov @ `0x532b56`. NOT modelled: test/je/compare/destroy @ `0x532b60..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR13MemRsp40_532b5b_U1Rva = 0x532b5b;
inline constexpr std::size_t kInit531d60PostBaseMovR13MemRsp40_532b5b_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR13MemRsp40_532b5b_U1EndRva = 0x532b60;

struct Init531d60PostBaseMovR13MemRsp40_532b5b_U1Cells {
    const void* const* rsp40_in = nullptr;
    void** r13_out = nullptr;
};

struct Init531d60PostBaseMovR13MemRsp40_532b5b_U1Result {
    bool ok = false;
    int mov_r13_mem_rsp40_count = 0;
    void* r13_loaded = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR13MemRsp40_532b5b_U1Result
init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1(
    const Init531d60PostBaseMovR13MemRsp40_532b5b_U1Cells& cells = {}) noexcept;

}  // namespace sotc
