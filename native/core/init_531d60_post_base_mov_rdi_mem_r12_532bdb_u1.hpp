// Init `0x531d60` registry mov rdi,[r12] OBS `0x532bdb..0x532bdf` (4 B):
//   mov rdi,[r12]   ; reload the old registry object base before the delete call
//
// After E613 call 0x170c0 @ `0x532bd6` (the `je 0x532bdb` target). NOT modelled:
// E613 mov r13,[0x15fe518] @ `0x532bdf` and the tail @ `0x532be6..`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovRdiMemR12_532bdb_U1Rva = 0x532bdb;
inline constexpr std::size_t kInit531d60PostBaseMovRdiMemR12_532bdb_U1Size = 4;
inline constexpr std::uint64_t kInit531d60PostBaseMovRdiMemR12_532bdb_U1EndRva = 0x532bdf;

struct Init531d60PostBaseMovRdiMemR12_532bdb_U1Cells {
    // Host stand-in for the qword at [r12] (the object base pointer). Null => rdi = null.
    void* const* cell_ptr = nullptr;
    void** rdi_out = nullptr;
};

struct Init531d60PostBaseMovRdiMemR12_532bdb_U1Result {
    bool ok = false;
    int mov_rdi_mem_r12_532bdb_count = 0;
    void* rdi = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovRdiMemR12_532bdb_U1Result
init_531d60_post_base_mov_rdi_mem_r12_532bdb_u1(
    const Init531d60PostBaseMovRdiMemR12_532bdb_U1Cells& cells = {}) noexcept;

}  // namespace sotc
