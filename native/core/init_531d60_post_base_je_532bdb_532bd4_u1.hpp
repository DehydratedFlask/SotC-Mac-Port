// Init `0x531d60` registry je null-rdi skip OBS `0x532bd4..0x532bd6` (2 B rel8):
//   je 0x532bdb   ; ZF from test rdi,rdi @ `0x532bd1`
//
// After E598 test @ `0x532bd1`. Fall-through @ `0x532bd6` is next call site.
// NOT modelled: `0x532bdb` epilogue / tail @ `0x532bd6`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseJe532bdb_532bd4_U1Rva = 0x532bd4;
inline constexpr std::size_t kInit531d60PostBaseJe532bdb_532bd4_U1Size = 2;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bdb_532bd4_U1EndRva = 0x532bd6;
inline constexpr std::uint64_t kInit531d60PostBaseJe532bdb_532bd4_U1TargetRva = 0x532bdb;

struct Init531d60PostBaseJe532bdb_532bd4_U1Cells {
    bool rdi_is_zero = false;
    bool rdi_is_zero_known = false;
};

struct Init531d60PostBaseJe532bdb_532bd4_U1Result {
    bool ok = false;
    int je_532bdb_count = 0;
    bool branch_outcome_modelled = false;
    bool branch_taken = false;
    std::uint64_t branch_target_rva = kInit531d60PostBaseJe532bdb_532bd4_U1TargetRva;
};

[[nodiscard]] Init531d60PostBaseJe532bdb_532bd4_U1Result init_531d60_post_base_je_532bdb_532bd4_u1(
    const Init531d60PostBaseJe532bdb_532bd4_U1Cells& cells = {}) noexcept;

}  // namespace sotc
