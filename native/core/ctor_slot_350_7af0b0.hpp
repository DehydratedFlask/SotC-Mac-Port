// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 350 (RVA 0x7af0b0, 610 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 350) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_350.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: two float variables sharing the caller functor 0x15ce260 and the empty description 0x1286db4.
//   A (object 0x325b5b0) `nico_PhotoModeLwsZoomControlSpeed`: BOUNDED via 0xc300, min 0x3dcccccd, max
//     0x40800000 (4.0), default and value 0x3f000000 (0.5).
//   B (object 0x325b688) `shadow_bowZoomScaleOnHorseback`: UNBOUNDED via 0x6aa00, default and value 0x3f800000.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot350Rva = 0x7af0b0;
inline constexpr std::size_t kSlot350Size = 610;
inline constexpr FloatVarSpec kSlot350SpecA{"nico_PhotoModeLwsZoomControlSpeed", "", true, 0x3dcccccdu, 0x40800000u,
                                            0x3f000000u, kFloatArgFnVtbl_350, 1};
inline constexpr FloatVarSpec kSlot350SpecB{"shadow_bowZoomScaleOnHorseback", "", false, 0u, 0u, 0x3f800000u,
                                            kFloatArgFnVtbl_350, 1};

struct Slot350Objects {
    ConCmdRangedVar var_a;
    ConCmdRangedVar var_b;
};

struct Slot350Result {
    bool ok = false;
    FloatVarResult var_a{};
    FloatVarResult var_b{};
};

[[nodiscard]] Slot350Result init_slot350_7af0b0(Slot350Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
