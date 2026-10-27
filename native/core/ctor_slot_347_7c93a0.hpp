// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 347 (RVA 0x7c93a0, 370 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 347) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_347.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE bounded float variable through 0xc300 (object 0x325e200): `nico_AltDiveAttackHoldTime`, min 0.0,
// max 10.0 (0x41200000), default and value 0x3e19999a (0.15f), flags 1, caller argument functor 0x15ce318.
// Sequence: [obj] = 0x15942b0 ; 0xc300 ; f590(description) ; functor destroy (bare ret) ; [obj] = 0x15942b0 ;
// value = default ; 0x20d60 ; f590(name) ; __cxa_atexit(0x20f0).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot347Rva = 0x7c93a0;
inline constexpr std::size_t kSlot347Size = 370;
inline constexpr FloatVarSpec kSlot347Spec{"nico_AltDiveAttackHoldTime", "", true, 0x00000000u, 0x41200000u,
                                           0x3e19999au, kFloatArgFnVtbl_347, 1};

struct Slot347Objects {
    ConCmdRangedVar var;
};

struct Slot347Result {
    bool ok = false;
    FloatVarResult var{};
};

[[nodiscard]] Slot347Result init_slot347_7c93a0(Slot347Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
