// Init `0x531d60` registry call 0x54dc10 OBS `0x532b19..0x532b1e` (5 B):
//   call 0x54dc10   ; input-table base accessor (returns rax)
//
// After E550 mov r14,rax @ `0x532b16`. NOT modelled: mov rbx,rax @ `0x532b1e`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "input_table_551140.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall54dc10_532b19_U1Rva = 0x532b19;
inline constexpr std::size_t kInit531d60PostBaseCall54dc10_532b19_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall54dc10_532b19_U1EndRva = 0x532b1e;
inline constexpr std::uint64_t kInit531d60PostBaseCall54dc10_532b19_U1TargetRva = 0x54dc10;

struct Init531d60PostBaseCall54dc10_532b19_U1Cells {
    InputTable551140* table_in = nullptr;
    void** rax_out = nullptr;
};

struct Init531d60PostBaseCall54dc10_532b19_U1Result {
    bool ok = false;
    int call_54dc10_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseCall54dc10_532b19_U1Result init_531d60_post_base_call_54dc10_532b19_u1(
    const Init531d60PostBaseCall54dc10_532b19_U1Cells& cells = {}) noexcept;

}  // namespace sotc
