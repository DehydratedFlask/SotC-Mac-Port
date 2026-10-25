// App vslot `0x533100` streams attach OBS `0x533a4c..0x533bc5` (377 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x50] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100StreamsAttachRva = 0x533A4C;
inline constexpr std::size_t kVslot40533100StreamsAttachSize = 377;
inline constexpr std::uint64_t kVslot40StreamsAttachEndRva = 0x533BC5;
inline constexpr std::uint64_t kVslot40StreamsAttachSlotOff = 0x50;
inline constexpr std::uint64_t kVslot40StreamsAttachFlagOff = 0x50 + 8;  // *[f08+0x50]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kStreamsKeyLiteral[] = "$/sound/shadow/****/shadowstreams.crmp";
static_assert(sizeof(kStreamsKeyLiteral) - 1 == 38, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100StreamsAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100StreamsAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100StreamsAttachResult vslot_40_533100_streams_attach(
    const Vslot40_533100StreamsAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
