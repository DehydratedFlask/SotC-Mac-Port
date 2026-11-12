// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): native model of startup constructor slot 512 (RVA 0x1d5e80, 902
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 512. Shape: 2 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x26c10 group item(s)
// (bool variables in the shared init_bool_var_static order: 0x20af0 base ctor, mid vptr, field stores, caller
// functor tag 0x1599288, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)). Every body fact is byte-pinned by tools/lift/check_ctor_slot_512.py.
// NOTHING here executes, loads or calls an original RVA: the vtables, the functor tags and the destructor RVAs
// are IDENTITY TAGS, recorded and never run. Names, captions and labels are stack views (control == nullptr), so
// the 0xf590 releases are exact no-ops. The __stack_chk_guard compare is a hook (not modelled), as in every other
// lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"
#include "ctor_slot_family_items.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot512Rva = 0x1d5e80;
inline constexpr std::size_t kSlot512Size = 902;
inline constexpr std::uint64_t kSlot512BoolFnTag = 0x1599288;
inline constexpr char kSlot512Var0Name[] = "staticModelTouch_enableTouchJobs";
inline constexpr std::size_t kSlot512Var0NameLength = 0x20;
inline constexpr char kSlot512Group0Parent[] = "";
inline constexpr std::size_t kSlot512Group0ParentLength = 0x0;
inline constexpr char kSlot512Group0Caption[] = "Static Touch Manager";
inline constexpr std::size_t kSlot512Group0CaptionLength = 0x14;
inline constexpr char kSlot512Item0Label[] = "Static Touch Manager";
inline constexpr std::size_t kSlot512Item0LabelLength = 0x14;
inline constexpr char kSlot512Item0Text[] = "Enable Static Model Touch Jobs";
inline constexpr std::size_t kSlot512Item0TextLength = 0x1e;
inline constexpr char kSlot512Var1Name[] = "staticModelTouch_disableGrid";
inline constexpr std::size_t kSlot512Var1NameLength = 0x1c;
inline constexpr char kSlot512Item1Label[] = "Static Touch Manager";
inline constexpr std::size_t kSlot512Item1LabelLength = 0x14;
inline constexpr char kSlot512Item1Text[] = "Disable Grid For Tests";
inline constexpr std::size_t kSlot512Item1TextLength = 0x16;
inline constexpr std::uint32_t kSlot512GroupFlags = 0x30;

struct Slot512Objects {
    ConCmdBoolVar var0;   // 0x2ee7a10
    DebugMenuItem group0;   // 0x2ee7ad8
    DebugMenuItem item0;   // 0x2ee7b80, target = var0
    ConCmdBoolVar var1;   // 0x2ee7c28
    DebugMenuItem item1;   // 0x2ee7cf0, target = var1
};

struct Slot512Result {
    bool ok = false;
    BoolVarResult var0{};
    FamilyItemResult group0{};
    MenuSiteResult item0{};
    BoolVarResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot512Result init_slot512_1d5e80(Slot512Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
