// App vslot `0x533100` banks attach OBS `0x5338d3..0x533a4c` (377 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[f08+0x40] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100BanksAttachRva = 0x5338D3;
inline constexpr std::size_t kVslot40533100BanksAttachSize = 377;
inline constexpr std::uint64_t kVslot40BanksAttachEndRva = 0x533A4C;
inline constexpr std::uint64_t kVslot40BanksAttachSlotOff = 0x40;
inline constexpr std::uint64_t kVslot40BanksAttachFlagOff = 0x40 + 8;  // *[f08+0x40]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kBanksKeyLiteral[] = "$/sound/shadow/****/shadowbanks.crmp";
static_assert(sizeof(kBanksKeyLiteral) - 1 == 36, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100BanksAttachHooks {
    Vslot40AttachCommonHooks common{};
};

using Vslot40_533100BanksAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100BanksAttachResult vslot_40_533100_banks_attach(
    const Vslot40_533100BanksAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
