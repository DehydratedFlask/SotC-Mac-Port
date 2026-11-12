// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): native model of startup constructor slot 338 (RVA 0x7e6c90, 2025
// bytes). Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 338. Shape: 9 BOOL console variable(s)
// (bool variables in the shared init_bool_var_static order: 0x20af0 base ctor, mid vptr, field stores, caller
// functor tag 0x15ce460, f590(description), final vptr, value/default byte, 0x20d60, f590(name),
// __cxa_atexit(0x20b0, var)). Every body fact is byte-pinned by tools/lift/check_ctor_slot_338.py.
// NOTHING here executes, loads or calls an original RVA: the vtables, the functor tags and the destructor RVAs
// are IDENTITY TAGS, recorded and never run. Names, captions and labels are stack views (control == nullptr), so
// the 0xf590 releases are exact no-ops. The __stack_chk_guard compare is a hook (not modelled), as in every other
// lane-M6 bool slot.
#pragma once

#include <cstddef>
#include <cstdint>

#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot338Rva = 0x7e6c90;
inline constexpr std::size_t kSlot338Size = 2025;
inline constexpr std::uint64_t kSlot338BoolFnTag = 0x15ce460;
inline constexpr char kSlot338Var0Name[] = "shadow_startWithAllItems";
inline constexpr std::size_t kSlot338Var0NameLength = 0x18;
inline constexpr char kSlot338Var1Name[] = "shadow_startWithWhistleArrowSet";
inline constexpr std::size_t kSlot338Var1NameLength = 0x1f;
inline constexpr char kSlot338Var2Name[] = "shadow_startWithExplosionArrowSet";
inline constexpr std::size_t kSlot338Var2NameLength = 0x21;
inline constexpr char kSlot338Var3Name[] = "shadow_startWithHarpoonSet";
inline constexpr std::size_t kSlot338Var3NameLength = 0x1a;
inline constexpr char kSlot338Var4Name[] = "shadow_startWithParachuteSet";
inline constexpr std::size_t kSlot338Var4NameLength = 0x1c;
inline constexpr char kSlot338Var5Name[] = "shadow_startWithSwordOfDorminSet";
inline constexpr std::size_t kSlot338Var5NameLength = 0x20;
inline constexpr char kSlot338Var6Name[] = "shadow_startWithSwordOfLifeSet";
inline constexpr std::size_t kSlot338Var6NameLength = 0x1e;
inline constexpr char kSlot338Var7Name[] = "shadow_startWithAncientBowSet";
inline constexpr std::size_t kSlot338Var7NameLength = 0x1d;
inline constexpr char kSlot338Var8Name[] = "shadow_startWithIcoTabbardSet";
inline constexpr std::size_t kSlot338Var8NameLength = 0x1d;

struct Slot338Objects {
    ConCmdBoolVar var0;   // 0x3260310
    ConCmdBoolVar var1;   // 0x32603d8
    ConCmdBoolVar var2;   // 0x32604a0
    ConCmdBoolVar var3;   // 0x3260568
    ConCmdBoolVar var4;   // 0x3260630
    ConCmdBoolVar var5;   // 0x32606f8
    ConCmdBoolVar var6;   // 0x32607c0
    ConCmdBoolVar var7;   // 0x3260888
    ConCmdBoolVar var8;   // 0x3260950
};

struct Slot338Result {
    bool ok = false;
    BoolVarResult var0{};
    BoolVarResult var1{};
    BoolVarResult var2{};
    BoolVarResult var3{};
    BoolVarResult var4{};
    BoolVarResult var5{};
    BoolVarResult var6{};
    BoolVarResult var7{};
    BoolVarResult var8{};
};

[[nodiscard]] Slot338Result init_slot338_7e6c90(Slot338Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
