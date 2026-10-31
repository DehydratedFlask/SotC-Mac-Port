// Init `0x531d60` registry tail — OBS @ `0x532b11..0x532b19` (2 units after E550 peel).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_call_17070_532b11_u1.hpp"
#include "init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
#include "init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailRva = 0x532b11;
inline constexpr std::size_t kInit531d60PostBaseRegistryTailSize = 8;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailEndRva = 0x532b19;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailFullSpanRva = 0x532b0c;
inline constexpr std::size_t kInit531d60PostBaseRegistryTailFullSpanSize = 13;

static_assert(kInit531d60PostBaseMovEdi98_532b0c_U1EndRva == kInit531d60PostBaseRegistryTailRva);
static_assert(kInit531d60PostBaseCall17070_532b11_U1Rva == kInit531d60PostBaseRegistryTailRva);
static_assert(kInit531d60PostBaseCall17070_532b11_U1EndRva ==
                  kInit531d60PostBaseMovR14Rax_532b16_U1Rva);
static_assert(kInit531d60PostBaseMovR14Rax_532b16_U1EndRva ==
                  kInit531d60PostBaseRegistryTailEndRva);

enum class RegistryTailUnit : int { call_17070_532b11_u1, mov_r14_rax_532b16_u1, count };
inline constexpr int kInit531d60PostBaseRegistryTailUnitCount =
    static_cast<int>(RegistryTailUnit::count);

struct Init531d60PostBaseRegistryTailHooks {
    bool merge_mov_edi_98_532b0c_u1_ran = false;
    bool merge_call_17070_532b11_u1_ran = false;
    bool merge_mov_r14_rax_532b16_u1_ran = false;
    void (*call_17070)() = nullptr;
    Init531d60PostBaseMovR14Rax_532b16_U1Cells mov_r14_rax_cells{};
};

struct Init531d60PostBaseRegistryTailResult {
    bool ok = false;
    std::array<int, 2> unit_counts{};
    int unit_executed = 0;
    int mov_edi_count = 0;
    int call_17070_count = 0;
    int mov_r14_rax_count = 0;
    std::uint64_t end_rva = kInit531d60PostBaseRegistryTailEndRva;
};

[[nodiscard]] Init531d60PostBaseRegistryTailResult init_531d60_post_base_registry_tail(
    const Init531d60PostBaseRegistryTailHooks& hooks = {}) noexcept;

}  // namespace sotc
