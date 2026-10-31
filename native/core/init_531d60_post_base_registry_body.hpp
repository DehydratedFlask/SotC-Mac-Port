// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532b29` (4 units after E554 peel).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_call_54dc10_532b19_u1.hpp"
#include "init_531d60_post_base_call_54dc20_532b21_u1.hpp"
#include "init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"
#include "init_531d60_post_base_mov_r15_rax_532b26_u1.hpp"
#include "init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
#include "input_table_551140.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyRva = 0x532b19;
inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 16;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b29;

static_assert(kInit531d60PostBaseMovR14Rax_532b16_U1EndRva == kInit531d60PostBaseRegistryBodyRva);
static_assert(kInit531d60PostBaseCall54dc10_532b19_U1Rva == kInit531d60PostBaseRegistryBodyRva);
static_assert(kInit531d60PostBaseCall54dc10_532b19_U1EndRva ==
                  kInit531d60PostBaseMovRbxRax_532b1e_U1Rva);
static_assert(kInit531d60PostBaseMovRbxRax_532b1e_U1EndRva ==
                  kInit531d60PostBaseCall54dc20_532b21_U1Rva);
static_assert(kInit531d60PostBaseCall54dc20_532b21_U1EndRva ==
                  kInit531d60PostBaseMovR15Rax_532b26_U1Rva);
static_assert(kInit531d60PostBaseMovR15Rax_532b26_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);

enum class RegistryBodyUnit : int {
    call_54dc10_532b19_u1,
    mov_rbx_rax_532b1e_u1,
    call_54dc20_532b21_u1,
    mov_r15_rax_532b26_u1,
    count
};
inline constexpr int kInit531d60PostBaseRegistryBodyUnitCount =
    static_cast<int>(RegistryBodyUnit::count);

struct Init531d60PostBaseRegistryBodyHooks {
    bool merge_call_54dc10_532b19_u1_ran = false;
    bool merge_mov_rbx_rax_532b1e_u1_ran = false;
    bool merge_call_54dc20_532b21_u1_ran = false;
    bool merge_mov_r15_rax_532b26_u1_ran = false;
    InputTable551140* table_in = nullptr;
    Init531d60PostBaseCall54dc10_532b19_U1Cells call_54dc10_cells{};
    Init531d60PostBaseMovRbxRax_532b1e_U1Cells mov_rbx_rax_cells{};
    Init531d60PostBaseCall54dc20_532b21_U1Cells call_54dc20_cells{};
    Init531d60PostBaseMovR15Rax_532b26_U1Cells mov_r15_rax_cells{};
};

struct Init531d60PostBaseRegistryBodyResult {
    bool ok = false;
    std::array<int, 4> unit_counts{};
    int unit_executed = 0;
    int call_54dc10_count = 0;
    int mov_rbx_rax_count = 0;
    int call_54dc20_count = 0;
    int mov_r15_rax_count = 0;
    void* rax_from_54dc10 = nullptr;
    void* rbx_from_54dc10 = nullptr;
    std::uint32_t count_from_54dc20 = 0;
    std::uint64_t r15_from_54dc20 = 0;
    std::uint64_t end_rva = kInit531d60PostBaseRegistryBodyEndRva;
};

[[nodiscard]] Init531d60PostBaseRegistryBodyResult init_531d60_post_base_registry_body(
    const Init531d60PostBaseRegistryBodyHooks& hooks = {}) noexcept;

}  // namespace sotc
