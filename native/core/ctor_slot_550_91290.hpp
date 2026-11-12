// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): native model of startup constructor slot 550 (RVA 0x91290, 706
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 550. Shape: 1 BOOL console variable(s), 2 0x26ea0 toggle item(s), 1 0x30-byte name holder(s) (dtor 0x8ef00)
// (bool variables in the shared init_bool_var_static order: 0x20af0 base ctor, mid vptr, field stores, caller
// functor tag 0x1597680, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)). Every body fact is byte-pinned by tools/lift/check_ctor_slot_550.py.
// NOTHING here executes, loads or calls an original RVA: the vtables, the functor tags and the destructor RVAs
// are IDENTITY TAGS, recorded and never run. Names, captions and labels are stack views (control == nullptr), so
// the 0xf590 releases are exact no-ops. The __stack_chk_guard compare is a hook (not modelled), as in every other
// lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30

namespace sotc {

inline constexpr std::uint64_t kSlot550Rva = 0x91290;
inline constexpr std::size_t kSlot550Size = 706;
inline constexpr std::uint64_t kSlot550BoolFnTag = 0x1597680;
inline constexpr char kSlot550Var0Name[] = "r_showRenderContextMemoryStats";
inline constexpr std::size_t kSlot550Var0NameLength = 0x1e;
inline constexpr char kSlot550Item0Label[] = "RenderDebug";
inline constexpr std::size_t kSlot550Item0LabelLength = 0xb;
inline constexpr char kSlot550Item0Text[] = "Show Render Context Memory";
inline constexpr std::size_t kSlot550Item0TextLength = 0x1a;
inline constexpr char kSlot550Item1Label[] = "MemoryMisc";
inline constexpr std::size_t kSlot550Item1LabelLength = 0xa;
inline constexpr char kSlot550Item1Text[] = "Show Render Context Memory";
inline constexpr std::size_t kSlot550Item1TextLength = 0x1a;
inline constexpr char kSlot550Holder0Name[] = "r_threadRObjectNewAndFree";
inline constexpr std::size_t kSlot550Holder0NameLength = 0x19;
inline constexpr std::uint64_t kSlot550HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot550HolderVptr = 0x15976b0;   // 0x15976a0 + 0x10 (identity tag)

struct Slot550Objects {
    ConCmdBoolVar var0;   // 0x2ed33f8
    DebugMenuItem item0;   // 0x2ed34c0, target = var0
    DebugMenuItem item1;   // 0x2ed3568, target = var0
    NameHolder30 holder0;   // 0x2ed3610
};

struct Slot550Result {
    bool ok = false;
    BoolVarResult var0{};
    MenuSiteResult item0{};
    MenuSiteResult item1{};
    bool holder0_ok = false;
};

[[nodiscard]] Slot550Result init_slot550_91290(Slot550Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
