// App vslot `0x533100` skels attach OBS `0x5335e1..0x53375a` (377 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x10] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100SkelsAttachRva = 0x5335E1;
inline constexpr std::size_t kVslot40533100SkelsAttachSize = 377;
inline constexpr std::uint64_t kVslot40SkelsAttachEndRva = 0x53375A;
inline constexpr std::uint64_t kVslot40SkelsAttachSlotOff = 0x10;
inline constexpr std::uint64_t kVslot40SkelsAttachFlagOff = 0x10 + 8;  // *[f08+0x10]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kSkelsKeyLiteral[] = "$/Models/****/shadowskels.crmp";
static_assert(sizeof(kSkelsKeyLiteral) - 1 == 30, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100SkelsAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100SkelsAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100SkelsAttachResult vslot_40_533100_skels_attach(
    const Vslot40_533100SkelsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
