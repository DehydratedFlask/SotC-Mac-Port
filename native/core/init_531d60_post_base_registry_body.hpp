// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532b88` (27 units after E577 peel).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "allocator_front.hpp"
#include "init_531d60_post_base_call_52a220_532b2e_u1.hpp"
#include "init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"
#include "init_531d60_post_base_cmp_r12_r14_532b44_u1.hpp"
#include "init_531d60_post_base_je_532bfb_532b47_u1.hpp"
#include "init_531d60_post_base_test_r12_r12_532b4d_u1.hpp"
#include "init_531d60_post_base_je_532bf8_532b50_u1.hpp"
#include "init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.hpp"
#include "init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.hpp"
#include "init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.hpp"
#include "init_531d60_post_base_cmp_r13_r14_532b65_u1.hpp"
#include "init_531d60_post_base_je_532bba_532b68_u1.hpp"
#include "init_531d60_post_base_lea_rsi_rsp10_532b6a_u1.hpp"
#include "init_531d60_post_base_mov_rbx_r13_10_532b70_u1.hpp"
#include "init_531d60_post_base_mov_r15_r13_18_532b74_u1.hpp"
#include "init_531d60_post_base_cmp_r15_r11_532b78_u1.hpp"
#include "init_531d60_post_base_je_532b98_532b7b_u1.hpp"
#include "init_531d60_post_base_nop_532b7d_u1.hpp"
#include "init_531d60_post_base_mov_rax_rbx_48_532b80_u1.hpp"
#include "init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.hpp"
#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"
#include "init_531d60_post_base_mov_r13_15fe518_532b3a_u1.hpp"
#include "init_531d60_post_base_call_54dc10_532b19_u1.hpp"
#include "init_531d60_post_base_call_54dc20_532b21_u1.hpp"
#include "init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"
#include "init_531d60_post_base_mov_r15_rax_532b26_u1.hpp"
#include "init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
#include "init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"
#include "init_531d60_post_base_mov_rsi_rbx_532b29_u1.hpp"
#include "input_table_551140.hpp"
#include "registry_object_2efd160.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyRva = 0x532b19;
inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 111;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b88;

static_assert(kInit531d60PostBaseMovR14Rax_532b16_U1EndRva == kInit531d60PostBaseRegistryBodyRva);
static_assert(kInit531d60PostBaseCall54dc10_532b19_U1Rva == kInit531d60PostBaseRegistryBodyRva);
static_assert(kInit531d60PostBaseCall54dc10_532b19_U1EndRva ==
                  kInit531d60PostBaseMovRbxRax_532b1e_U1Rva);
static_assert(kInit531d60PostBaseMovRbxRax_532b1e_U1EndRva ==
                  kInit531d60PostBaseCall54dc20_532b21_U1Rva);
static_assert(kInit531d60PostBaseCall54dc20_532b21_U1EndRva ==
                  kInit531d60PostBaseMovR15Rax_532b26_U1Rva);
static_assert(kInit531d60PostBaseMovR15Rax_532b26_U1EndRva ==
                  kInit531d60PostBaseMovRsiRbx_532b29_U1Rva);
static_assert(kInit531d60PostBaseMovRsiRbx_532b29_U1EndRva ==
                  kInit531d60PostBaseMovEdxEax_532b2c_U1Rva);
static_assert(kInit531d60PostBaseMovEdxEax_532b2c_U1EndRva ==
                  kInit531d60PostBaseCall52a220_532b2e_U1Rva);
static_assert(kInit531d60PostBaseCall52a220_532b2e_U1EndRva ==
                  kInit531d60PostBaseLeaRbx2efd160_532b33_U1Rva);
static_assert(kInit531d60PostBaseLeaRbx2efd160_532b33_U1EndRva ==
                  kInit531d60PostBaseMovR13_15fe518_532b3a_U1Rva);
static_assert(kInit531d60PostBaseMovR13_15fe518_532b3a_U1EndRva ==
                  kInit531d60PostBaseMovR12Rbx_532b41_U1Rva);
static_assert(kInit531d60PostBaseMovR12Rbx_532b41_U1EndRva ==
                  kInit531d60PostBaseCmpR12R14_532b44_U1Rva);
static_assert(kInit531d60PostBaseCmpR12R14_532b44_U1EndRva ==
                  kInit531d60PostBaseJe532bfb_532b47_U1Rva);
static_assert(kInit531d60PostBaseJe532bfb_532b47_U1EndRva ==
                  kInit531d60PostBaseTestR12R12_532b4d_U1Rva);
static_assert(kInit531d60PostBaseTestR12R12_532b4d_U1EndRva ==
                  kInit531d60PostBaseJe532bf8_532b50_U1Rva);
static_assert(kInit531d60PostBaseJe532bf8_532b50_U1EndRva ==
                  kInit531d60PostBaseMovR14MemRsp10_532b56_U1Rva);
static_assert(kInit531d60PostBaseMovR14MemRsp10_532b56_U1EndRva ==
                  kInit531d60PostBaseMovR13MemRsp40_532b5b_U1Rva);
static_assert(kInit531d60PostBaseMovR13MemRsp40_532b5b_U1EndRva ==
                  kInit531d60PostBaseMovR14MemRsp48_532b60_U1Rva);
static_assert(kInit531d60PostBaseMovR14MemRsp48_532b60_U1EndRva ==
                  kInit531d60PostBaseCmpR13R14_532b65_U1Rva);
static_assert(kInit531d60PostBaseCmpR13R14_532b65_U1EndRva ==
                  kInit531d60PostBaseJe532bba_532b68_U1Rva);
static_assert(kInit531d60PostBaseJe532bba_532b68_U1EndRva ==
                  kInit531d60PostBaseLeaRsiRsp10_532b6a_U1Rva);
static_assert(kInit531d60PostBaseMovRbxR13_10_532b70_U1Rva ==
                  kInit531d60PostBaseLeaRsiRsp10_532b6a_U1EndRva + 1);
static_assert(kInit531d60PostBaseMovRbxR13_10_532b70_U1EndRva ==
                  kInit531d60PostBaseMovR15R13_18_532b74_U1Rva);
static_assert(kInit531d60PostBaseMovR15R13_18_532b74_U1EndRva ==
                  kInit531d60PostBaseCmpR15R11_532b78_U1Rva);
static_assert(kInit531d60PostBaseCmpR15R11_532b78_U1EndRva ==
                  kInit531d60PostBaseJe532b98_532b7b_U1Rva);
static_assert(kInit531d60PostBaseJe532b98_532b7b_U1EndRva ==
                  kInit531d60PostBaseNop532b7d_U1Rva);
static_assert(kInit531d60PostBaseNop532b7d_U1EndRva ==
                  kInit531d60PostBaseMovRaxRbx_48_532b80_U1Rva);
static_assert(kInit531d60PostBaseMovRaxRbx_48_532b80_U1EndRva ==
                  kInit531d60PostBaseLeaRdiRbx_28_532b84_U1Rva);
static_assert(kInit531d60PostBaseLeaRdiRbx_28_532b84_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);

enum class RegistryBodyUnit : int {
    call_54dc10_532b19_u1,
    mov_rbx_rax_532b1e_u1,
    call_54dc20_532b21_u1,
    mov_r15_rax_532b26_u1,
    mov_rsi_rbx_532b29_u1,
    mov_edx_eax_532b2c_u1,
    call_52a220_532b2e_u1,
    lea_rbx_2efd160_532b33_u1,
    mov_r13_15fe518_532b3a_u1,
    mov_r12_rbx_532b41_u1,
    cmp_r12_r14_532b44_u1,
    je_532bfb_532b47_u1,
    test_r12_r12_532b4d_u1,
    je_532bf8_532b50_u1,
    mov_r14_mem_rsp10_532b56_u1,
    mov_r13_mem_rsp40_532b5b_u1,
    mov_r14_mem_rsp48_532b60_u1,
    cmp_r13_r14_532b65_u1,
    je_532bba_532b68_u1,
    lea_rsi_rsp10_532b6a_u1,
    mov_rbx_r13_10_532b70_u1,
    mov_r15_r13_18_532b74_u1,
    cmp_r15_r11_532b78_u1,
    je_532b98_532b7b_u1,
    nop_532b7d_u1,
    mov_rax_rbx_48_532b80_u1,
    lea_rdi_rbx_28_532b84_u1,
    count
};
inline constexpr int kInit531d60PostBaseRegistryBodyUnitCount =
    static_cast<int>(RegistryBodyUnit::count);

struct Init531d60PostBaseRegistryBodyHooks {
    bool merge_call_54dc10_532b19_u1_ran = false;
    bool merge_mov_rbx_rax_532b1e_u1_ran = false;
    bool merge_call_54dc20_532b21_u1_ran = false;
    bool merge_mov_r15_rax_532b26_u1_ran = false;
    bool merge_mov_rsi_rbx_532b29_u1_ran = false;
    bool merge_mov_edx_eax_532b2c_u1_ran = false;
    bool merge_call_52a220_532b2e_u1_ran = false;
    bool merge_lea_rbx_2efd160_532b33_u1_ran = false;
    bool merge_mov_r13_15fe518_532b3a_u1_ran = false;
    bool merge_mov_r12_rbx_532b41_u1_ran = false;
    bool merge_cmp_r12_r14_532b44_u1_ran = false;
    bool merge_je_532bfb_532b47_u1_ran = false;
    bool merge_test_r12_r12_532b4d_u1_ran = false;
    bool merge_je_532bf8_532b50_u1_ran = false;
    bool merge_mov_r14_mem_rsp10_532b56_u1_ran = false;
    bool merge_mov_r13_mem_rsp40_532b5b_u1_ran = false;
    bool merge_mov_r14_mem_rsp48_532b60_u1_ran = false;
    bool merge_cmp_r13_r14_532b65_u1_ran = false;
    bool merge_je_532bba_532b68_u1_ran = false;
    bool merge_lea_rsi_rsp10_532b6a_u1_ran = false;
    bool merge_mov_rbx_r13_10_532b70_u1_ran = false;
    bool merge_mov_r15_r13_18_532b74_u1_ran = false;
    bool merge_cmp_r15_r11_532b78_u1_ran = false;
    bool merge_je_532b98_532b7b_u1_ran = false;
    bool merge_nop_532b7d_u1_ran = false;
    bool merge_mov_rax_rbx_48_532b80_u1_ran = false;
    bool merge_lea_rdi_rbx_28_532b84_u1_ran = false;
    const void* stack_rsp = nullptr;
    InputTable551140* table_in = nullptr;
    RegistryObject2efd160* obj_in = nullptr;
    void* r14_in = nullptr;
    AllocatorFront* alloc = nullptr;
    Init531d60PostBaseCall54dc10_532b19_U1Cells call_54dc10_cells{};
    Init531d60PostBaseMovRbxRax_532b1e_U1Cells mov_rbx_rax_cells{};
    Init531d60PostBaseCall54dc20_532b21_U1Cells call_54dc20_cells{};
    Init531d60PostBaseMovR15Rax_532b26_U1Cells mov_r15_rax_cells{};
    Init531d60PostBaseMovRsiRbx_532b29_U1Cells mov_rsi_rbx_cells{};
    Init531d60PostBaseMovEdxEax_532b2c_U1Cells mov_edx_eax_cells{};
    Init531d60PostBaseCall52a220_532b2e_U1Cells call_52a220_cells{};
    Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells lea_rbx_2efd160_cells{};
    Init531d60PostBaseMovR13_15fe518_532b3a_U1Cells mov_r13_15fe518_cells{};
    Init531d60PostBaseMovR12Rbx_532b41_U1Cells mov_r12_rbx_cells{};
    Init531d60PostBaseCmpR12R14_532b44_U1Cells cmp_r12_r14_cells{};
    Init531d60PostBaseJe532bfb_532b47_U1Cells je_532bfb_cells{};
    Init531d60PostBaseTestR12R12_532b4d_U1Cells test_r12_r12_cells{};
    Init531d60PostBaseJe532bf8_532b50_U1Cells je_532bf8_cells{};
    Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells mov_r14_mem_rsp10_cells{};
    Init531d60PostBaseMovR13MemRsp40_532b5b_U1Cells mov_r13_mem_rsp40_cells{};
    Init531d60PostBaseMovR14MemRsp48_532b60_U1Cells mov_r14_mem_rsp48_cells{};
    Init531d60PostBaseCmpR13R14_532b65_U1Cells cmp_r13_r14_cells{};
    Init531d60PostBaseJe532bba_532b68_U1Cells je_532bba_cells{};
    Init531d60PostBaseLeaRsiRsp10_532b6a_U1Cells lea_rsi_rsp10_cells{};
    Init531d60PostBaseMovRbxR13_10_532b70_U1Cells mov_rbx_r13_10_cells{};
    Init531d60PostBaseMovR15R13_18_532b74_U1Cells mov_r15_r13_18_cells{};
    Init531d60PostBaseCmpR15R11_532b78_U1Cells cmp_r15_r11_cells{};
    Init531d60PostBaseJe532b98_532b7b_U1Cells je_532b98_cells{};
    Init531d60PostBaseNop532b7d_U1Cells nop_532b7d_cells{};
    Init531d60PostBaseMovRaxRbx_48_532b80_U1Cells mov_rax_rbx_48_cells{};
    Init531d60PostBaseLeaRdiRbx_28_532b84_U1Cells lea_rdi_rbx_28_cells{};
};

struct Init531d60PostBaseRegistryBodyResult {
    bool ok = false;
    std::array<int, 27> unit_counts{};
    int unit_executed = 0;
    int call_54dc10_count = 0;
    int mov_rbx_rax_count = 0;
    int call_54dc20_count = 0;
    int mov_r15_rax_count = 0;
    int mov_rsi_rbx_count = 0;
    int mov_edx_eax_count = 0;
    int call_52a220_count = 0;
    int lea_rbx_2efd160_count = 0;
    int mov_r13_15fe518_count = 0;
    int mov_r12_rbx_count = 0;
    int cmp_r12_r14_count = 0;
    int je_532bfb_count = 0;
    int test_r12_r12_count = 0;
    int je_532bf8_count = 0;
    int mov_r14_mem_rsp10_count = 0;
    int mov_r13_mem_rsp40_count = 0;
    int mov_r14_mem_rsp48_count = 0;
    int cmp_r13_r14_count = 0;
    int je_532bba_count = 0;
    int lea_rsi_rsp10_count = 0;
    int mov_rbx_r13_10_count = 0;
    int mov_r15_r13_18_count = 0;
    int cmp_r15_r11_count = 0;
    int je_532b98_count = 0;
    int nop_532b7d_count = 0;
    int mov_rax_rbx_48_count = 0;
    int lea_rdi_rbx_28_count = 0;
    void* rax_from_rbx_48 = nullptr;
    void* rdi_from_rbx_28 = nullptr;
    bool r12_r14_equal = false;
    bool r13_r14_equal = false;
    bool r15_r11_equal = false;
    bool r12_is_zero = false;
    bool je_branch_taken = false;
    bool je_branch_outcome_modelled = false;
    bool je_532bf8_branch_taken = false;
    bool je_532bf8_branch_outcome_modelled = false;
    bool je_532bba_branch_taken = false;
    bool je_532bba_branch_outcome_modelled = false;
    bool je_532b98_branch_taken = false;
    bool je_532b98_branch_outcome_modelled = false;
    void* rsi_from_rsp10_lea = nullptr;
    void* rbx_from_r13_10 = nullptr;
    void* r15_from_r13_18 = nullptr;
    void* r13_from_guard_cell = nullptr;
    void* r12_from_cell = nullptr;
    void* rax_from_54dc10 = nullptr;
    void* rbx_from_54dc10 = nullptr;
    void* rsi_from_rbx = nullptr;
    void* rbx_global_cell_addr = nullptr;
    std::uint32_t edx_from_eax = 0;
    std::uint32_t count_from_54dc20 = 0;
    std::uint64_t r15_from_54dc20 = 0;
    bool fill_invoked = false;
    std::uint64_t end_rva = kInit531d60PostBaseRegistryBodyEndRva;
};

[[nodiscard]] Init531d60PostBaseRegistryBodyResult init_531d60_post_base_registry_body(
    const Init531d60PostBaseRegistryBodyHooks& hooks = {}) noexcept;

}  // namespace sotc
