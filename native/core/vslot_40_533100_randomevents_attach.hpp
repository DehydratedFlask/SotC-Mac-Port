// App vslot `0x533100` randomevents attach OBS `0x533bc5..0x533d3e` (377 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x60] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100RandomeventsAttachRva = 0x533BC5;
inline constexpr std::size_t kVslot40533100RandomeventsAttachSize = 377;
inline constexpr std::uint64_t kVslot40RandomeventsAttachEndRva = 0x533D3E;
inline constexpr std::uint64_t kVslot40RandomeventsAttachSlotOff = 0x60;
inline constexpr std::uint64_t kVslot40RandomeventsAttachFlagOff = 0x60 + 8;  // *[f08+0x60]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kRandomeventsKeyLiteral[] = "$/sound/shadow/****/shadowrandomevents.crmp";
static_assert(sizeof(kRandomeventsKeyLiteral) - 1 == 43, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100RandomeventsAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100RandomeventsAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100RandomeventsAttachResult vslot_40_533100_randomevents_attach(
    const Vslot40_533100RandomeventsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
