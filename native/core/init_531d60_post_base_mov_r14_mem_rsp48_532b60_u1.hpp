// Init `0x531d60` registry mov r14,[rsp+0x48] OBS `0x532b60..0x532b65` (5 B):
//   mov r14,[rsp+0x48]   ; destroy-path prologue after mov r13,[rsp+0x40] @ `0x532b5b`
//
// After E566 mov @ `0x532b5b`. NOT modelled: je/destroy @ `0x532b68..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp48_532b60_U1Rva = 0x532b60;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRsp48_532b60_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp48_532b60_U1EndRva = 0x532b65;

struct Init531d60PostBaseMovR14MemRsp48_532b60_U1Cells {
    const void* const* rsp48_in = nullptr;
    void** r14_out = nullptr;
};

struct Init531d60PostBaseMovR14MemRsp48_532b60_U1Result {
    bool ok = false;
    int mov_r14_mem_rsp48_count = 0;
    void* r14_loaded = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRsp48_532b60_U1Result
init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1(
    const Init531d60PostBaseMovR14MemRsp48_532b60_U1Cells& cells = {}) noexcept;

}  // namespace sotc
