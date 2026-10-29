// CPU-01-STARTUP-SLOT-441 (lane m3): native model of startup constructor slot 441 (RVA 0x4e68c0, 410 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 441. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_441.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape, in order (the slot 344 var + toggle pair, without its cells and command): (1) bool variable `cp10_debugSwordLight`
// (object 0x2efbf08, tag 0x15c2c20, value 0, default 0, flags 1); (2) a debug-menu toggle built by the
// 0x26db0 constructor (item 0x2efbfd0) under the parent path `Shadow Player`, its caption taken from the variable's
// name; flags 0x30; f590(parent); atexit(0x272d0). The 0x268b0 registration is the tail of 0x26db0 and already
// native (init_toggle_site_26db0). Objects are addressed rip-relative.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot441Rva = 0x4e68c0;
inline constexpr std::size_t kSlot441Size = 410;
inline constexpr char kSlot441VarName[] = "cp10_debugSwordLight";
inline constexpr char kSlot441Parent[] = "Shadow Player";
inline constexpr std::uint64_t kSlot441FnTag = 0x15c2c20;
inline constexpr std::uint64_t kSlot441ObjectRva = 0x2efbf08;
inline constexpr std::uint64_t kSlot441ItemRva = 0x2efbfd0;

struct Slot441Objects {
    ConCmdBoolVar var;       // 0x2efbf08
    DebugMenuItem item;      // 0x2efbfd0
};

struct Slot441Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot441Result init_slot441_4e68c0(Slot441Objects& objects, ConCmdRegistry& concmd,
                                 DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
