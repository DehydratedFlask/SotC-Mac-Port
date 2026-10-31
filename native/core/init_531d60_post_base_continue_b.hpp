// Init `0x531d60` continue B OBS `0x532793..0x5327b1` (30 B, 8 consecutive units):
//   0x532793  mov ebx,eax                 (2 B)
//   0x532795  mov r12b,bl                 (3 B)
//   0x532798  xor r12b,1                  (4 B)
//   0x53279c  movzx edi,r12b              (4 B)
//   0x5327a0  call 0x4c4950               (5 B)
//   0x5327a5  mov rdi,[r13+0]             (4 B)
//   0x5327a9  test bl,bl                  (2 B)
//   0x5327ab  je 0x5328e0                 (6 B)
//
// Always-on after E514 first `0x1e4c0` call-site (0x53278e..0x532793).
// G1 remains PARTIAL. PR #5 cite-only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueBRva = 0x532793;
inline constexpr std::size_t kInit531d60PostBaseContinueBSize = 30;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBEndRva = 0x5327b1;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBBranchRva = 0x5327ab;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBBranchTargetRva = 0x5328e0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBCall4c4950Rva = 0x4c4950;
inline constexpr std::uint64_t kInit531d60PostBaseContinueBR13CellRva = 0x32f84f8;

enum class ContinueBUnit : int {
    mov_ebx_eax,        // 0x532793
    mov_r12b_bl,        // 0x532795
    xor_r12b_1,         // 0x532798
    movzx_edi_r12b,     // 0x53279c
    call_4c4950,        // 0x5327a0
    mov_rdi_r13_load,   // 0x5327a5
    test_bl_bl,         // 0x5327a9
    je_5328e0,          // 0x5327ab
    count
};

inline constexpr int kInit531d60PostBaseContinueBUnitCount =
    static_cast<int>(ContinueBUnit::count);

struct ContinueBUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

inline constexpr std::array<ContinueBUnitSpan, kInit531d60PostBaseContinueBUnitCount>
    kInit531d60PostBaseContinueBUnits{{
        {0x532793, 2, "mov ebx,eax"},
        {0x532795, 3, "mov r12b,bl"},
        {0x532798, 4, "xor r12b,1"},
        {0x53279c, 4, "movzx edi,r12b"},
        {0x5327a0, 5, "call 0x4c4950"},
        {0x5327a5, 4, "mov rdi,[r13+0]"},
        {0x5327a9, 2, "test bl,bl"},
        {0x5327ab, 6, "je 0x5328e0"},
    }};

struct Init531d60PostBaseContinueBHooks {
    int incoming_eax_from_1e4c0_u1 = 0;
    bool incoming_eax_known = false;
    const void* const* cell_32f84f8 = nullptr;
    void (*call_4c4950)(std::uint8_t edi) = nullptr;
    // Used only by run_init_continue_ab_native_lookups (routed through 1e4c0_u1).
    int (*call_1e4c0)(void* rdi) = nullptr;
};

struct Init531d60PostBaseContinueBResult {
    bool ok = false;
    std::array<int, kInit531d60PostBaseContinueBUnitCount> unit_counts{};
    int unit_executed = 0;         // expect 8
    int call_count = 0;            // expect 1
    int alu_count = 0;             // expect 2
    int load_count = 0;            // expect 0..1
    int branch_boundary_count = 0; // expect 1
    int eax_from_1e4c0 = 0;
    bool eax_known = false;
    std::uint64_t end_rva = kInit531d60PostBaseContinueBEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueBBranchTargetRva;
    bool branch_outcome_modelled = false;
    std::uint8_t bl = 0;
    bool branch_taken = false;
};

[[nodiscard]] Init531d60PostBaseContinueBResult init_531d60_post_base_continue_b(
    const Init531d60PostBaseContinueBHooks& hooks = {}) noexcept;

}  // namespace sotc
