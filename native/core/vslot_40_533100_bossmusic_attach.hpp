// App vslot `0x533100` bossmusic attach OBS `0x533d3e..0x533eac` (366 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x70] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100BossmusicAttachRva = 0x533D3E;
inline constexpr std::size_t kVslot40533100BossmusicAttachSize = 366;
inline constexpr std::uint64_t kVslot40BossmusicAttachEndRva = 0x533EAC;
inline constexpr std::uint64_t kVslot40BossmusicAttachSlotOff = 0x70;
inline constexpr std::uint64_t kVslot40BossmusicAttachFlagOff = 0x70 + 8;  // *[f08+0x70]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kBossmusicKeyLiteral[] = "$/sound/shadow/****/shadowbossmusic.crmp";
static_assert(sizeof(kBossmusicKeyLiteral) - 1 == 40, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100BossmusicAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100BossmusicAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100BossmusicAttachResult vslot_40_533100_bossmusic_attach(
    const Vslot40_533100BossmusicAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
