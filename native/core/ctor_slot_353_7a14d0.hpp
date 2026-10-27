// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 353 (RVA 0x7a14d0, 569 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 353) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_353.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: a bool variable (A, object 0x325a428, `shadow_camera_normal_overrideDefaultPitchOffset`, tag 0x15ce1a0,
// value 1, default 1) followed by an UNBOUNDED float variable through 0x6aa00 (B, object 0x325a4f0,
// `shadow_camera_normal_defaultPitchOffset`, functor 0x15ce1c0, flags 1, default and value 0). Both share the
// empty description 0x128640f.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot353Rva = 0x7a14d0;
inline constexpr std::size_t kSlot353Size = 569;
inline constexpr char kSlot353BoolName[] = "shadow_camera_normal_overrideDefaultPitchOffset";
inline constexpr std::uint64_t kSlot353BoolFnTag = 0x15ce1a0;
inline constexpr FloatVarSpec kSlot353FloatSpec{"shadow_camera_normal_defaultPitchOffset", "", false, 0u, 0u, 0u,
                                                kFloatArgFnVtbl_353B, 1};

struct Slot353Objects {
    ConCmdBoolVar flag;
    ConCmdRangedVar offset;
};

struct Slot353Result {
    bool ok = false;
    BoolVarResult flag{};
    FloatVarResult offset{};
};

[[nodiscard]] Slot353Result init_slot353_7a14d0(Slot353Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
