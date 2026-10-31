// NOT modelled: vtable load semantics; callee body beyond lifted 0x54b250 wrapper. G1 PARTIAL.
#pragma once
#include "stat_query_54b250.hpp"
#include <cstddef>
#include <cstdint>
namespace sotc {
inline constexpr std::uint64_t kInit531d60PostBaseVcall08_532b09_U1Rva = 0x532b09;
inline constexpr std::size_t kInit531d60PostBaseVcall08_532b09_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseVcall08_532b09_U1EndRva = 0x532b0c;
inline constexpr std::uint64_t kInit531d60PostBaseVcall08_532b09_U1ResolvedTargetRva = 0x54b250;
struct Init531d60PostBaseVcall08_532b09_U1Hooks {
    void* this_obj = nullptr;
    void (*call_54b250)(void*, std::uint32_t, std::uint32_t) = nullptr;
    const StatsEngine54b250* stat_engine = nullptr;
};
struct Init531d60PostBaseVcall08_532b09_U1Result {
    bool ok = false;
    int vcall_54b250_count = 0;
    bool stat_query_ran = false;
    StatQuery54b250Result stat_query{};
};
[[nodiscard]] Init531d60PostBaseVcall08_532b09_U1Result
init_531d60_post_base_vcall08_532b09_u1(const Init531d60PostBaseVcall08_532b09_U1Hooks& hooks = {}) noexcept;
}
