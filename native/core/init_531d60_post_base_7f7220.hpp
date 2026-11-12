// Init `0x531d60` continue call-site OBS `0x531e3c..0x531e41` (5 B):
//   call 0x7f7220
//   // stop before next call 0x56fad0 @ 0x531e41
//
// Always-on after E453 AVX BSS copy. Target 0x7f7220 is the staffroll path
// helper (~108 B to first ret; selects staffroll_*.txt) — claim the call-site,
// not the body. Prefer over deepen 0x1290 / 0x5c64c0 / 0x56dcb0 / 0x499f80 /
// 0x140d90 / 0x14c2f0 / 0x6a5370. Prefer over empty-path migrate/non-null.
//
// Claimed: call-site wrapper. The 54 B body is native in init_post_bodies.cpp; the runtime
// supplies the query/set hooks and executes both observed branches' selection here (E1426).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBase7f7220Rva = 0x531e3c;
inline constexpr std::size_t kInit531d60PostBase7f7220Size = 5;
inline constexpr std::uint64_t kInit531d60PostBase7f7220EndRva = 0x531e41;
inline constexpr std::uint64_t kInit531d60PostBase7f7220TargetRva = 0x7f7220;

struct Init531d60PostBase7f7220Hooks {
    // Models 0x7f7220() (staffroll helper body NOT lifted). Null → no-op.
    void (*call_7f7220)() = nullptr;
};

struct Init531d60PostBase7f7220Result {
    bool ok = false;
    int call_7f7220_count = 0;
};

// Models OBS 0x531e3c..0x531e41 on measured Init success path.
[[nodiscard]] Init531d60PostBase7f7220Result init_531d60_post_base_7f7220(
    const Init531d60PostBase7f7220Hooks& hooks = {}) noexcept;

}  // namespace sotc
