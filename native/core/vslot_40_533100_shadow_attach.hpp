// App vslot `0x533100` shadow attach OBS `0x53347f..0x533582` (259 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100ShadowAttachRva = 0x53347F;
inline constexpr std::size_t kVslot40533100ShadowAttachSize = 259;
inline constexpr std::uint64_t kVslot40ShadowAttachEndRva = 0x533582;
inline constexpr std::uint64_t kVslot40ShadowAttachSlotOff = 0x0;
inline constexpr std::uint64_t kVslot40ShadowAttachFlagOff = 0x0 + 8;  // *[f08+0]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kShadowKeyLiteral[] = "$/Models/****/shadowmodels.crmp";
static_assert(sizeof(kShadowKeyLiteral) - 1 == 31, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100ShadowAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100ShadowAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100ShadowAttachResult vslot_40_533100_shadow_attach(
    const Vslot40_533100ShadowAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
