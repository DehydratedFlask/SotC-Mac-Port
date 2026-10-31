// Init `0x531d60` continue OBS `0x53297f..0x53299d` (30 B, 8 consecutive units).
//
// Merge tail after E518 `0xa431e0` call-site u1; prep `0x53294b..0x53297a` demoted.
// Third vtable slot-0 dispatch, flag load, merge `je 0x5329ae` boundary.
//
// Claimed: span, 8 units, one vcall, two global loads, je target 0x5329ae.
// NOT modelled: callee bodies, je successors, incoming register provenance.
// G1 remains PARTIAL.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "lazy_bind_91210.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseContinueCRva = 0x53297f;
inline constexpr std::size_t kInit531d60PostBaseContinueCSize = 30;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCEndRva = 0x53299d;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCBranchRva = 0x53299b;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCBranchTargetRva = 0x5329ae;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCCallA431e0Rva = 0xa431e0;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCGlobal2efe318Rva = 0x2efe318;
inline constexpr std::uint64_t kInit531d60PostBaseContinueCGlobal2efe340Rva = 0x2efe340;

enum class ContinueCUnit : int {
    mov_rax_2efe318_2 = 0,  // 0x53297f
    xor_esi_esi_3,          // 0x532986
    mov_rdi_r15_2,          // 0x532988
    vcall_slot0_2,          // 0x53298b
    mov_rax_2efe340_2,      // 0x53298d
    mov_rax_rax60_2,        // 0x532994
    cmp_byte_rax0_2,        // 0x532998
    je_5329ae,              // 0x53299b
    count
};

inline constexpr int kInit531d60PostBaseContinueCUnitCount =
    static_cast<int>(ContinueCUnit::count);

struct ContinueCUnitSpan {
    std::uint64_t rva = 0;
    std::uint8_t size = 0;
    const char* mnemonic = "";
};

inline constexpr std::array<ContinueCUnitSpan, kInit531d60PostBaseContinueCUnitCount>
    kInit531d60PostBaseContinueCUnits{{
        {0x53297f, 7, "mov rax,[0x2efe318]"},
        {0x532986, 2, "xor esi,esi"},
        {0x532988, 3, "mov rdi,r15"},
        {0x53298b, 2, "call [rax]"},
        {0x53298d, 7, "mov rax,[0x2efe340]"},
        {0x532994, 4, "mov rax,[rax+0x60]"},
        {0x532998, 3, "cmp byte [rax],0"},
        {0x53299b, 2, "je 0x5329ae"},
    }};

struct Init531d60PostBaseContinueCHooks {
    std::uint8_t incoming_r14b = 0;
    bool enter_via_5328e0 = false;
    const std::uint8_t* preamble_flag_byte = nullptr;
    const void* incoming_r15 = nullptr;

    const void* const* cell_32f8b80 = nullptr;
    const void* const* cell_32f6d18 = nullptr;
    const void* const* cell_2efe318 = nullptr;
    const void* const* cell_2efe340 = nullptr;

    const std::uint8_t* flag_byte_iter1 = nullptr;
    const std::uint8_t* flag_byte_iter2 = nullptr;

    void (*call_a52b70)(void* rdi, std::uint8_t esi) = nullptr;
    bool merge_a52b70_u1_ran = false;
    void (*call_a599e0)(void* rdi, std::uint8_t esi) = nullptr;
    bool merge_a599e0_u1_ran = false;
    void (*call_a431e0)(void* rdi, std::uint8_t esi) = nullptr;
    bool merge_a431e0_u1_ran = false;
    void (*vcall_slot0)(void* rdi, std::uint8_t esi) = nullptr;
    const std::uint8_t* merge_flag_byte = nullptr;

    bool use_native_lazy_bind_91210 = false;
    void* demo_object = nullptr;
    LazyBind91210Hooks lazy_bind{};
};

struct Init531d60PostBaseContinueCResult {
    bool ok = false;
    std::array<int, kInit531d60PostBaseContinueCUnitCount> unit_counts{};
    int unit_executed = 0;
    int call_count = 0;
    int vcall_count = 0;
    int lea_count = 0;
    int global_mov_count = 0;
    int xor_count = 0;
    int cmp_count = 0;
    int sete_count = 0;
    int branch_boundary_count = 0;
    bool flag1_known = false;
    bool flag2_known = false;
    std::uint8_t sil_iter1 = 0;
    std::uint8_t sil_iter2 = 0;
    std::uint64_t end_rva = kInit531d60PostBaseContinueCEndRva;
    std::uint64_t branch_target_rva = kInit531d60PostBaseContinueCBranchTargetRva;
    bool branch_outcome_modelled = false;
    bool merge_branch_outcome_modelled = false;
    bool merge_branch_taken = false;
    bool preamble_ran = false;
    int preamble_unit_executed = 0;
    int preamble_call_count = 0;
    bool preamble_flag_known = false;
    std::uint8_t preamble_r14b = 0;
    std::uint8_t first_esi = 0;
    int lazy_bind_calls = 0;
    int lazy_bind_last_returned = 0;
    bool lazy_bind_ok = false;
};

[[nodiscard]] Init531d60PostBaseContinueCResult init_531d60_post_base_continue_c(
    const Init531d60PostBaseContinueCHooks& hooks = {}) noexcept;

}  // namespace sotc
