// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 415 (RVA 0x578b60, 331 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 415. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_415.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE unbounded int variable through the 0x3a980 worker (object 0x2f295e8): `isysLinkAllocMax`, flags 8
// (the other int-variable site, slot 334, passes 1), value and default 0, caller argument functor table 0x15cb128
// (four slots = the lifted int functor bodies 0xc720/0xc740/0xc760/0xc770, checker-pinned). Sequence:
// [obj] = 0x1594bf8+0x10 ; 0x3a980 ; f590(description) ; functor destroy (bare ret) ; [obj] = same ; value =
// default = 0 ; 0x20d60 ; f590(name) ; __cxa_atexit(0x18a60). What the value limits is NOT recovered (the name is
// the only evidence; unused here).
#pragma once

#include "lane_m2_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot415Rva = 0x578b60;
inline constexpr std::size_t kSlot415Size = 331;
inline constexpr std::uint64_t kSlot415ArgFnTable = 0x15cb128;
inline constexpr std::uint32_t kSlot415CtorFlags = 8;
inline constexpr char kSlot415VarName[] = "isysLinkAllocMax";
inline constexpr lane_m2::IntVarSpec kSlot415Spec{kSlot415VarName, "", 0x15cb128, 8, 0, 0};

struct Slot415Objects {
    ConCmdRangedVar var;     // 0x2f295e8
};

struct Slot415Result {
    bool ok = false;
    lane_m2::IntVarResult var{};
};

[[nodiscard]] Slot415Result init_slot415_578b60(Slot415Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
