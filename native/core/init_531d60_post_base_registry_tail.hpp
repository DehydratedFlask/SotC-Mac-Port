// Init `0x531d60` registry tail — OBS @ `0x532b11` (0 B pinned units after E548 peel).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailRva = 0x532b11;
inline constexpr std::size_t kInit531d60PostBaseRegistryTailSize = 0;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailEndRva = 0x532b11;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailFullSpanRva = 0x532b0c;
inline constexpr std::size_t kInit531d60PostBaseRegistryTailFullSpanSize = 6;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailCall17070Rva = 0x532b11;
inline constexpr std::uint64_t kInit531d60PostBaseRegistryTailCall17070TargetRva = 0x17070;

static_assert(kInit531d60PostBaseMovEdi98_532b0c_U1EndRva ==
                  kInit531d60PostBaseRegistryTailCall17070Rva);

enum class RegistryTailUnit : int { count = 0 };
inline constexpr int kInit531d60PostBaseRegistryTailUnitCount =
    static_cast<int>(RegistryTailUnit::count);

struct Init531d60PostBaseRegistryTailHooks {
    bool merge_mov_edi_98_532b0c_u1_ran = false;
};

struct Init531d60PostBaseRegistryTailResult {
    bool ok = false;
    std::array<int, 1> unit_counts{};
    int unit_executed = 0;
    int mov_edi_count = 0;
    std::uint64_t end_rva = kInit531d60PostBaseRegistryTailEndRva;
};

[[nodiscard]] Init531d60PostBaseRegistryTailResult init_531d60_post_base_registry_tail(
    const Init531d60PostBaseRegistryTailHooks& hooks = {}) noexcept;

}  // namespace sotc
