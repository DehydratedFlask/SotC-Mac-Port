// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): native model of startup constructor slot 443 (RVA 0x4de370, 936
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 443. Shape: 2 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x26c10 group item(s)
// (bool variables in the shared init_bool_var_static order: 0x20af0 base ctor, mid vptr, field stores, caller
// functor tag 0x15c2400, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)). Every body fact is byte-pinned by tools/lift/check_ctor_slot_443.py.
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

inline constexpr std::uint64_t kSlot443Rva = 0x4de370;
inline constexpr std::size_t kSlot443Size = 936;
inline constexpr std::uint64_t kSlot443BoolFnTag = 0x15c2400;
inline constexpr char kSlot443Var0Name[] = "showHandledScriptEvents";
inline constexpr std::size_t kSlot443Var0NameLength = 0x17;
inline constexpr char kSlot443Var1Name[] = "showAllScriptEvents";
inline constexpr std::size_t kSlot443Var1NameLength = 0x13;
inline constexpr char kSlot443Group0Parent[] = "CP10";
inline constexpr std::size_t kSlot443Group0ParentLength = 0x4;
inline constexpr char kSlot443Group0Caption[] = "Scripted Events";
inline constexpr std::size_t kSlot443Group0CaptionLength = 0xf;
inline constexpr char kSlot443Item0Label[] = "Scripted Events";
inline constexpr std::size_t kSlot443Item0LabelLength = 0xf;
inline constexpr char kSlot443Item0Text[] = "Show Handled Events";
inline constexpr std::size_t kSlot443Item0TextLength = 0x13;
inline constexpr char kSlot443Item1Label[] = "Scripted Events";
inline constexpr std::size_t kSlot443Item1LabelLength = 0xf;
inline constexpr char kSlot443Item1Text[] = "Show All Events";
inline constexpr std::size_t kSlot443Item1TextLength = 0xf;
inline constexpr std::uint32_t kSlot443GroupFlags = 0x30;

struct Slot443Objects {
    ConCmdBoolVar var0;   // 0x2efb908
    ConCmdBoolVar var1;   // 0x2efb9d0
    DebugMenuItem group0;   // 0x2efba98
    DebugMenuItem item0;   // 0x2efbb40, target = var0
    DebugMenuItem item1;   // 0x2efbbe8, target = var1
};

struct Slot443Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    FamilyItemResult group0{};
    MenuSiteResult item0{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot443Result init_slot443_4de370(Slot443Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
