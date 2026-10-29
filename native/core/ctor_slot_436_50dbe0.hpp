// CPU-01-STARTUP-SLOT-436 (lane m3): native model of startup constructor slot 436 (RVA 0x50dbe0, 410 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 436. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_436.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape, in order (the slot 344 var + toggle pair, without its cells and command): (1) bool variable `shadow_showDOFStats`
// (object 0x2efc750, tag 0x15c6498, value 0, default 0, flags 1); (2) a debug-menu toggle built by the
// 0x26db0 constructor (item 0x2efc818) under the parent path `Depth of Field`, its caption taken from the variable's
// name; flags 0x30; f590(parent); atexit(0x272d0). The 0x268b0 registration is the tail of 0x26db0 and already
// native (init_toggle_site_26db0). Objects are addressed rip-relative.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot436Rva = 0x50dbe0;
inline constexpr std::size_t kSlot436Size = 410;
inline constexpr char kSlot436VarName[] = "shadow_showDOFStats";
inline constexpr char kSlot436Parent[] = "Depth of Field";
inline constexpr std::uint64_t kSlot436FnTag = 0x15c6498;
inline constexpr std::uint64_t kSlot436ObjectRva = 0x2efc750;
inline constexpr std::uint64_t kSlot436ItemRva = 0x2efc818;

struct Slot436Objects {
    ConCmdBoolVar var;       // 0x2efc750
    DebugMenuItem item;      // 0x2efc818
};

struct Slot436Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot436Result init_slot436_50dbe0(Slot436Objects& objects, ConCmdRegistry& concmd,
                                 DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
