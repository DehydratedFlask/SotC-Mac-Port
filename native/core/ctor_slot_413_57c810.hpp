// CPU-01-STARTUP-SLOTS-M2 (lane M2): native model of startup constructor slot 413 (RVA 0x57c810, 370 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 413. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_413.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bounded float variable through 0xc300 (object 0x2f358c8): `shadow_deadZoneDefault`, min 0.0, max 1.0
// (0x3f800000), default and value 0x3e780347 (about 0.2422f), flags 1, caller argument functor 0x15cb148 (four slots
// = the lifted float functor bodies 0xc4a0/0xc4c0/0xc4e0/0xc4f0, checker-pinned). Sequence: [obj] = 0x15942b0 ;
// 0xc300 ; f590(description) ; functor destroy (bare ret) ; [obj] = 0x15942b0 ; value = default ; 0x20d60 ;
// f590(name) ; __cxa_atexit(0x20f0). The functor tag is accepted by lane_m2::init_float_var_site (see
// core/lane_m2_shapes.hpp for why that is output-neutral). Whether this is a controller dead zone is NOT
// established (inference only from the name).
#pragma once

#include "lane_m2_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot413Rva = 0x57c810;
inline constexpr std::size_t kSlot413Size = 370;
inline constexpr std::uint64_t kSlot413FnTag = 0x15cb148;
inline constexpr char kSlot413VarName[] = "shadow_deadZoneDefault";
inline constexpr FloatVarSpec kSlot413Spec{kSlot413VarName, "", true, 0x00000000u, 0x3f800000u, 0x3e780347u,
                                           0x15cb148, 1};

struct Slot413Objects {
    ConCmdRangedVar var;     // 0x2f358c8
};

struct Slot413Result {
    bool ok = false;
    FloatVarResult var{};
};

[[nodiscard]] Slot413Result init_slot413_57c810(Slot413Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
