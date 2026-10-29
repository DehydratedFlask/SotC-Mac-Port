// CPU-01-STARTUP-SLOT-507 (lane m6): native model of startup constructor slot 507 (RVA 0x22f3c0, 567 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 507. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_507.py. NOTHING here
// executes, loads or calls an original RVA: the vtables, the functor tag 0x159b500 and the destructor 0x20b0 are
// IDENTITY TAGS, recorded and never run.
//
// Observed shape (stack canary present): two BOOL console variables back to back (0xc8 bytes apart), each built in
// exactly the order of the shared init_bool_var_static sequence (0x20af0, mid vptr, fields, caller functor tag
// 0x159b500, f590(description), final vptr, value 1, default 1, 0x20d60, f590(name), __cxa_atexit(0x20b0, var)):
//   * 0x2ee92c0  `ExhaustiveDegenerateSplineSegmentLookahead`  (name length 0x2a)
//   * 0x2ee9388  `BruteForcePathSmoothing`                     (name length 0x17, = first variable + 0xc8)
// Both have an EMPTY description (the shared empty string at 0xfc00fd) and are enabled by default (1 / 1). Name and
// description are stack views with control == nullptr, so the 0xf590 releases are exact no-ops. The tag 0x159b500's
// four relocation slots are the bool functor bodies (checker).
// Inference (not observed): the names suggest path-spline tuning flags; no consumer is lifted here.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot507Rva = 0x22f3c0;
inline constexpr std::size_t kSlot507Size = 567;
inline constexpr std::uint64_t kSlot507BoolFnTag = 0x159b500;
inline constexpr char kSlot507Var1Name[] = "ExhaustiveDegenerateSplineSegmentLookahead";
inline constexpr std::size_t kSlot507Var1NameLength = 0x2a;
inline constexpr char kSlot507Var2Name[] = "BruteForcePathSmoothing";
inline constexpr std::size_t kSlot507Var2NameLength = 0x17;

struct Slot507Objects {
    ConCmdBoolVar lookahead;   // 0x2ee92c0
    ConCmdBoolVar smoothing;   // 0x2ee9388
};

struct Slot507Result {
    bool ok = false;
    BoolVarResult first{};
    BoolVarResult second{};
};

[[nodiscard]] Slot507Result init_slot507_22f3c0(Slot507Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
