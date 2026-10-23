// App vslot `0x533100` anims attach OBS `0x53375a..0x533870` (278 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x20] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100AnimsAttachRva = 0x53375A;
inline constexpr std::size_t kVslot40533100AnimsAttachSize = 278;
inline constexpr std::uint64_t kVslot40AnimsAttachEndRva = 0x533870;
inline constexpr std::uint64_t kVslot40AnimsAttachSlotOff = 0x20;
inline constexpr std::uint64_t kVslot40AnimsAttachFlagOff = 0x20 + 8;  // *[f08+0x20]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kAnimsKeyLiteral[] = "$/Models/****/shadowanims.crmp";
static_assert(sizeof(kAnimsKeyLiteral) - 1 == 30, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100AnimsAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100AnimsAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100AnimsAttachResult vslot_40_533100_anims_attach(
    const Vslot40_533100AnimsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
