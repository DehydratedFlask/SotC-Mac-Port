// App vslot `0x533100` cp3characters attach OBS `0x533eac..0x533f97` (235 B): production model.
//
// The whole recovered block is modelled by `vslot_40_533100_attach_common` (E595):
//   key 0x14c2f0 -> GetResource -> AddReference -> slot join -> two RemoveReference -> key
//   0xf590 -> Lock(slot, 13).
// The earlier reading of `[out+8]` as a caller "success flag" and of the Lock as a notify is
// withdrawn: `[out]`/`[out+8]` are the GetResource `{ptr, owned}` result, and `owned` is always
// false in this build. The persistent slot at *[self+0x3cb8] is the only owner.
//
// Key literal and extent are byte-pinned; the shared implementation carries the control flow.
#pragma once

#include <cstddef>
#include <cstdint>

#include "vslot_40_533100_attach_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kVslot40533100Cp3charactersAttachRva = 0x533EAC;
inline constexpr std::size_t kVslot40533100Cp3charactersAttachSize = 235;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachEndRva = 0x533F97;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachSlotOff = 0x3CB8;
inline constexpr std::uint64_t kVslot40Cp3charactersAttachFlagOff = 0x3CB8 + 8;  // *[self+0x3cb8]

// 0x14c2f0 literal (E582 §5.1); `****` is replaced by the runtime platform tag before hashing.
inline constexpr const char kCp3charactersKeyLiteral[] = "$/characters/shadow/****/cp3Characters.calt";
static_assert(sizeof(kCp3charactersKeyLiteral) - 1 == 43, "0x14c2f0 literal length (E582 §5.1)");

struct Vslot40_533100Cp3charactersAttachHooks {
    Vslot40AttachCommonHooks common{};
    // The app object for the inlined self-relative slot variant.
    void* self = nullptr;
};

using Vslot40_533100Cp3charactersAttachResult = Vslot40AttachCommonResult;

// Runs the attach block. A missing pool or slot is a visible failure (`ok == false`).
[[nodiscard]] Vslot40_533100Cp3charactersAttachResult vslot_40_533100_cp3characters_attach(
    const Vslot40_533100Cp3charactersAttachHooks& hooks = {}) noexcept;

}  // namespace sotc
