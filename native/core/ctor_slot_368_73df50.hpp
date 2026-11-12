// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): native model of startup constructor slot 368 (RVA 0x73df50, 1317
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 368. Shape: 3 BOOL console variable(s), 3 0x26ea0 toggle item(s), then 18 literal dword stores into static cells
// (bool variables in the shared init_bool_var_static order: 0x20af0 base ctor, mid vptr, field stores, caller
// functor tag 0x15cd1a0, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)). Every body fact is byte-pinned by tools/lift/check_ctor_slot_368.py.
// NOTHING here executes, loads or calls an original RVA: the vtables, the functor tags and the destructor RVAs
// are IDENTITY TAGS, recorded and never run. Names, captions and labels are stack views (control == nullptr), so
// the 0xf590 releases are exact no-ops. The __stack_chk_guard compare is a hook (not modelled), as in every other
// lane-M6 bool slot. The trailing literal stores have no lifted consumer: the cells are modelled as plain dwords.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot368Rva = 0x73df50;
inline constexpr std::size_t kSlot368Size = 1317;
inline constexpr std::uint64_t kSlot368BoolFnTag = 0x15cd1a0;
inline constexpr char kSlot368Var0Name[] = "shadow_MungePlayerExpression";
inline constexpr std::size_t kSlot368Var0NameLength = 0x1c;
inline constexpr char kSlot368Var1Name[] = "shadow_DisableSwordHoistPlayerExpression";
inline constexpr std::size_t kSlot368Var1NameLength = 0x28;
inline constexpr char kSlot368Var2Name[] = "shadow_debugPlayerExpression";
inline constexpr std::size_t kSlot368Var2NameLength = 0x1c;
inline constexpr char kSlot368Item0Label[] = "Shadow IK";
inline constexpr std::size_t kSlot368Item0LabelLength = 0x9;
inline constexpr char kSlot368Item0Text[] = "Munge Player Expression";
inline constexpr std::size_t kSlot368Item0TextLength = 0x17;
inline constexpr char kSlot368Item1Label[] = "Shadow IK";
inline constexpr std::size_t kSlot368Item1LabelLength = 0x9;
inline constexpr char kSlot368Item1Text[] = "Disable sword hoist Player Expression";
inline constexpr std::size_t kSlot368Item1TextLength = 0x25;
inline constexpr char kSlot368Item2Label[] = "Shadow IK";
inline constexpr std::size_t kSlot368Item2LabelLength = 0x9;
inline constexpr char kSlot368Item2Text[] = "Debug Player Expression";
inline constexpr std::size_t kSlot368Item2TextLength = 0x17;
inline constexpr std::size_t kSlot368StaticStoreCount = 18;
struct Slot368StaticStore { std::uint64_t rva; std::uint32_t bits; };
inline constexpr Slot368StaticStore kSlot368StaticStores[kSlot368StaticStoreCount] = {
    {0x2dcc0ac, 0x00000000},
    {0x2dcc0f4, 0x3f000000},
    {0x2dcc824, 0xbfcccccd},
    {0x2dccda8, 0x00000000},
    {0x2dccdac, 0x00000000},
    {0x2dccdb0, 0x00000000},
    {0x2dcce80, 0x00000000},
    {0x2dcce84, 0x00000000},
    {0x2dcce88, 0x3f800000},
    {0x2dccc40, 0x3f800000},
    {0x2dccc44, 0x40400000},
    {0x2dcd254, 0x3ecccccd},
    {0x2dcc184, 0x00000000},
    {0x2dcc828, 0xc0833333},
    {0x2dccc48, 0xbf800000},
    {0x2dcd258, 0xc019999a},
    {0x2dcd620, 0xbf800000},
    {0x2dcd628, 0x3f666666}};

struct Slot368Objects {
    ConCmdBoolVar var0;   // 0x3250cd0
    ConCmdBoolVar var1;   // 0x3250d98
    ConCmdBoolVar var2;   // 0x3250e60
    DebugMenuItem item0;   // 0x3250f28, target = var0
    DebugMenuItem item1;   // 0x3250fd0, target = var1
    DebugMenuItem item2;   // 0x3251078, target = var2
    std::uint32_t statics[kSlot368StaticStoreCount];   // the .bss/.data cells written by the tail, in store order
};

struct Slot368Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    MenuSiteResult item0{};
    MenuSiteResult item1{};
    MenuSiteResult item2{};
    std::size_t statics_written = 0;
};

[[nodiscard]] Slot368Result init_slot368_73df50(Slot368Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
