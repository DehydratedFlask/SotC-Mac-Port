// App vslot `0x533100` cp3shadowprecache attach OBS `0x533f97..0x53403c` (165 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 9).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[self+0x3cc8] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3shadowprecacheAttachRva = 0x533F97;
inline constexpr std::size_t kVslot40533100Cp3shadowprecacheAttachSize = 165;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachEndRva = 0x53403C;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachSlotOff = 0x3CC8;
inline constexpr std::uint64_t kVslot40Cp3shadowprecacheAttachFlagOff = 0x3CC8 + 8;  // *[self+0x3cc8]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kCp3shadowprecacheKeyLiteral[] = "$/cp3shadow/****/cp3shadowprecache.calt";
static_assert(sizeof(kCp3shadowprecacheKeyLiteral) - 1 == 39, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100Cp3shadowprecacheAttachHooks {
    Vslot40AttachCommonHooks common{};
    // The app object for the inlined self-relative slot variant.
    void* self = nullptr;
};

using Vslot40_533100Cp3shadowprecacheAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100Cp3shadowprecacheAttachResult vslot_40_533100_cp3shadowprecache_attach(
    const Vslot40_533100Cp3shadowprecacheAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
