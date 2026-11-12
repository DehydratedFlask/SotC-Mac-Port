// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 455 (RVA 0x47bfa0, 817 bytes). Original ELF
// sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 455. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_455.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the functor
// tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `looseAttachments_Enable` (object 0x2ef8250, tag 0x15bddb8, value 1, default 1, empty
// description, flags 1); (2) bool variable `looseAttachments_ShowCollision` (object 0x2ef8318, value 0, default 0);
// (3) a debug-menu GROUP item (0x26c10, item 0x2ef83e0) with parent path `Animation`, caption `Loose Attachment`, flags 0x20,
// f590(caption), f590(parent), atexit(0x272d0); (4)/(5) two 0x26db0 toggles (items 0x2ef8488, 0x2ef8530) for the two
// variables under the parent path `Loose Attachment`, flags 0x30, f590(parent), atexit(0x272d0). The 0x268b0 registration is
// the tail of each item constructor and already native; 0x26db0 is the bool-class adapter here (target class 0x1594218,
// slot 3 -> 1, slot 6 -> 0; tools/lift/check_helper_26db0.py), so there is no functor slot-3 call site. The
// __stack_chk_guard compare is a hook (not modelled).
//
// NOT established: what reads the two variables at run time. Reading from the strings: loose-attachment (cloth/dangling
// bone) enable and collision-display switches; INFERENCE only, no consumer is lifted.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_family_items.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot455Rva = 0x47bfa0;
inline constexpr std::size_t kSlot455Size = 817;
inline constexpr std::uint64_t kSlot455FnTag = 0x15bddb8;
inline constexpr char kSlot455Var0Name[] = "looseAttachments_Enable";
inline constexpr std::size_t kSlot455Var0NameLength = 0x17;
inline constexpr char kSlot455Var1Name[] = "looseAttachments_ShowCollision";
inline constexpr std::size_t kSlot455Var1NameLength = 0x1e;
inline constexpr char kSlot455GroupParent[] = "Animation";
inline constexpr std::size_t kSlot455GroupParentLength = 9;
inline constexpr char kSlot455GroupCaption[] = "Loose Attachment";
inline constexpr std::size_t kSlot455GroupCaptionLength = 0x10;
inline constexpr std::uint32_t kSlot455GroupFlags = 0x20;
inline constexpr std::uint64_t kSlot455Var0Rva = 0x2ef8250;
inline constexpr std::uint64_t kSlot455Var1Rva = 0x2ef8318;
inline constexpr std::uint64_t kSlot455GroupRva = 0x2ef83e0;
inline constexpr std::uint64_t kSlot455Item0Rva = 0x2ef8488;
inline constexpr std::uint64_t kSlot455Item1Rva = 0x2ef8530;

struct Slot455Objects {
    ConCmdBoolVar var0;      // 0x2ef8250
    ConCmdBoolVar var1;      // 0x2ef8318
    DebugMenuItem group;     // 0x2ef83e0
    DebugMenuItem item0;     // 0x2ef8488
    DebugMenuItem item1;     // 0x2ef8530
};

struct Slot455Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    FamilyItemResult group{};
    MenuSiteResult item0{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot455Result init_slot455_47bfa0(Slot455Objects& objects, ConCmdRegistry& concmd,
                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
