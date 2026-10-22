// App vslot `0x533100` post-header fragment OBS `0x5331cd..0x533207` (58 B):
//   lea cell 0x2efc9e0; call 0x525080(*cell)     // f08 singleton consumer (hook)
//   call 0x2569d0(self, arg)                     // heavy (hook)
//   lea name "show_areaInfo"; call 0x1fc80(self+8, name)
//   test rax; jne hit  (miss/hit bodies NOT modelled)
//
// Always-on after bounded header install. Prefer over ensure migrate / non-null
// list (still empty-path skipped on measured Init). Not full 3956 B body.
//
// Claimed: fragment control flow + name pin + 0x1fc80 lift (via lookup_1fc80).
// NOT modelled: bodies of 0x525080 / 0x2569d0; miss path (r_fps/0x21620/ba40);
// hit path @ 0x533305; ensure migrate; non-null list. PR #5 cite-only.
#pragma once

#include "lookup_1fc80.hpp"
#include "singleton_f08_531d84.hpp"

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot40533100PostRva = 0x5331cd;
inline constexpr std::size_t kVslot40533100PostSize = 58;  // through jne @ 0x533201
inline constexpr const char kVslot40PostNameShowAreaInfo[] = "show_areaInfo";

struct Vslot40_533100PostHooks {
    // Models 0x525080(rdi=*0x2efc9e0). Null → no-op.
    void (*call_525080)(void* f08_obj) = nullptr;
    // Models 0x2569d0(rdi=self, rsi=arg). Null → no-op.
    void (*call_2569d0)(void* self, void* arg) = nullptr;
    // Models 0x1fc80. If null and use_lifted_lookup, run lookup_1fc80.
    void* (*lookup_1fc80_override)(void* table, const char* name) = nullptr;
    bool use_lifted_lookup = true;
    Lookup1fc80Hooks lookup_detail{};
};

struct Vslot40_533100PostResult {
    bool ok = false;
    int call_525080_count = 0;
    int call_2569d0_count = 0;
    int lookup_count = 0;
    bool lookup_hit = false;
    void* lookup_node = nullptr;
};

// Models OBS 0x5331cd..0x533207. Stops at branch; miss/hit bodies NOT run.
[[nodiscard]] Vslot40_533100PostResult vslot_40_533100_post(
    void* self, void* arg, const Vslot40_533100PostHooks& hooks = {}) noexcept;

}  // namespace sotc
