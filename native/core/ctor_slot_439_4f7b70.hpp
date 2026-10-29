// CPU-01-STARTUP-SLOT-439 (lane m3): native model of startup constructor slot 439 (RVA 0x4f7b70, 410 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 439. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_439.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape, in order (the slot 344 var + toggle pair, without its cells and command): (1) bool variable `r_waterWetHelperDebugDraw`
// (object 0x2efc180, tag 0x15c4a50, value 0, default 0, flags 1); (2) a debug-menu toggle built by the
// 0x26db0 constructor (item 0x2efc248) under the parent path `Water`, its caption taken from the variable's
// name; flags 0x30; f590(parent); atexit(0x272d0). The 0x268b0 registration is the tail of 0x26db0 and already
// native (init_toggle_site_26db0). Objects are addressed rip-relative.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot439Rva = 0x4f7b70;
inline constexpr std::size_t kSlot439Size = 410;
inline constexpr char kSlot439VarName[] = "r_waterWetHelperDebugDraw";
inline constexpr char kSlot439Parent[] = "Water";
inline constexpr std::uint64_t kSlot439FnTag = 0x15c4a50;
inline constexpr std::uint64_t kSlot439ObjectRva = 0x2efc180;
inline constexpr std::uint64_t kSlot439ItemRva = 0x2efc248;

struct Slot439Objects {
    ConCmdBoolVar var;       // 0x2efc180
    DebugMenuItem item;      // 0x2efc248
};

struct Slot439Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot439Result init_slot439_4f7b70(Slot439Objects& objects, ConCmdRegistry& concmd,
                                 DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
