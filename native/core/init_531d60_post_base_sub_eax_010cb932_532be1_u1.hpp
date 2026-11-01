// Init `0x531d60` registry sub eax,0x010cb932 OBS `0x532be1..0x532be6` (5 B):
//   sub eax,0x010cb932   ; 32-bit adjust after mov rax,[rdi] @ `0x532bde`
//
// After E602 `mov_rax_rdi_mem_532bde_u1` end @ `0x532be1`. NOT modelled: tail @
// `je_532bf0_532be6_u1` @ `0x532be6` / tail @ `0x532beb`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseSubEax_010cb932_532be1_U1Rva = 0x532be1;
inline constexpr std::size_t kInit531d60PostBaseSubEax_010cb932_532be1_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseSubEax_010cb932_532be1_U1EndRva = 0x532be6;
inline constexpr std::uint32_t kInit531d60PostBaseSubEax_010cb932_532be1_U1Imm = 0x010cb932u;

struct Init531d60PostBaseSubEax_010cb932_532be1_U1Cells {
    std::uint32_t eax_in = 0;
    std::uint32_t* eax_out = nullptr;
};

struct Init531d60PostBaseSubEax_010cb932_532be1_U1Result {
    bool ok = false;
    int sub_eax_010cb932_532be1_count = 0;
    std::uint32_t eax = 0;
};

[[nodiscard]] Init531d60PostBaseSubEax_010cb932_532be1_U1Result
init_531d60_post_base_sub_eax_010cb932_532be1_u1(
    const Init531d60PostBaseSubEax_010cb932_532be1_U1Cells& cells = {}) noexcept;

}  // namespace sotc
