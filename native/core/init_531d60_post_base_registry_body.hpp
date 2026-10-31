// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532b3a` (8 units after E558 peel).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "allocator_front.hpp"
#include "init_531d60_post_base_call_52a220_532b2e_u1.hpp"
#include "init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"
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
inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 33;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b3a;

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
    InputTable551140* table_in = nullptr;
    RegistryObject2efd160* obj_in = nullptr;
    AllocatorFront* alloc = nullptr;
    Init531d60PostBaseCall54dc10_532b19_U1Cells call_54dc10_cells{};
    Init531d60PostBaseMovRbxRax_532b1e_U1Cells mov_rbx_rax_cells{};
    Init531d60PostBaseCall54dc20_532b21_U1Cells call_54dc20_cells{};
    Init531d60PostBaseMovR15Rax_532b26_U1Cells mov_r15_rax_cells{};
    Init531d60PostBaseMovRsiRbx_532b29_U1Cells mov_rsi_rbx_cells{};
    Init531d60PostBaseMovEdxEax_532b2c_U1Cells mov_edx_eax_cells{};
    Init531d60PostBaseCall52a220_532b2e_U1Cells call_52a220_cells{};
    Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells lea_rbx_2efd160_cells{};
};

struct Init531d60PostBaseRegistryBodyResult {
    bool ok = false;
    std::array<int, 8> unit_counts{};
    int unit_executed = 0;
    int call_54dc10_count = 0;
    int mov_rbx_rax_count = 0;
    int call_54dc20_count = 0;
    int mov_r15_rax_count = 0;
    int mov_rsi_rbx_count = 0;
    int mov_edx_eax_count = 0;
    int call_52a220_count = 0;
    int lea_rbx_2efd160_count = 0;
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
