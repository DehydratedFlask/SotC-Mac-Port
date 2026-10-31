// Init `0x531d60` registry call 0x52a220 OBS `0x532b2e..0x532b33` (5 B):
//   call 0x52a220   ; fill ctor (rdi=obj/r14, rsi=table base, rdx=count in edx)
//
// After E556 mov edx,eax @ `0x532b2c`. NOT modelled: store/compare @ `0x532b33..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "allocator_front.hpp"
#include "input_table_551140.hpp"
#include "registry_fill_52a220.hpp"
#include "registry_object_2efd160.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseCall52a220_532b2e_U1Rva = 0x532b2e;
inline constexpr std::size_t kInit531d60PostBaseCall52a220_532b2e_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseCall52a220_532b2e_U1EndRva = 0x532b33;
inline constexpr std::uint64_t kInit531d60PostBaseCall52a220_532b2e_U1TargetRva = 0x52a220;

struct Init531d60PostBaseCall52a220_532b2e_U1Cells {
    RegistryObject2efd160* obj_in = nullptr;
    InputTable551140* table_in = nullptr;
    AllocatorFront* alloc = nullptr;
    std::uint32_t edx_in = 0;
};

struct Init531d60PostBaseCall52a220_532b2e_U1Result {
    bool ok = false;
    int call_52a220_count = 0;
    RegistryFill52a220Status fill_status = RegistryFill52a220Status::ok;
    bool fill_invoked = false;
};

[[nodiscard]] Init531d60PostBaseCall52a220_532b2e_U1Result init_531d60_post_base_call_52a220_532b2e_u1(
    const Init531d60PostBaseCall52a220_532b2e_U1Cells& cells = {}) noexcept;

}  // namespace sotc
