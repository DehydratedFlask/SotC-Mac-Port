// Init `0x531d60` registry test rdi,rdi; je +5 OBS `0x532be6..0x532beb` (5 B):
//   test rdi,rdi
//   je 0x532bf0   ; skip call @ `0x532beb` when null
//
// After E603 `sub_eax_010cb932_532be1_u1` end @ `0x532be6`. Fall-through call is E605
// `call_170c0_532beb_u1` @ `0x532beb` / E606 `mov_rdi_r12_532bf0_u1` @ `0x532bf0` / E607 `call_170b0_532bf3_u1` @ `0x532bf3`. NOT modelled: E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bf0_532be6_U1Rva = 0x532be6;
inline constexpr std::size_t kInit531d60PostBaseJe532bf0_532be6_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bf0_532be6_U1EndRva = 0x532beb;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bf0_532be6_U1TargetRva = 0x532bf0;

struct Init531d60PostBaseJe532bf0_532be6_U1Cells {
    void* rdi_in = nullptr;
};

struct Init531d60PostBaseJe532bf0_532be6_U1Result {
    bool ok = false;
    int je_532bf0_532be6_count = 0;
    bool rdi_is_zero = false;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bf0_532be6_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bf0_532be6_U1Result init_531d60_post_base_je_532bf0_532be6_u1(
    const Init531d60PostBaseJe532bf0_532be6_U1Cells& cells = {}) noexcept;

}  // namespace sotc
