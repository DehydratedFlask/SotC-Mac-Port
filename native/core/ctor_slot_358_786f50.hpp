// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 358 (RVA 0x786f50, 1769 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 358. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_358.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Attract-mode page (inference from the strings): attractMovieEnabled / attractMovieTime / cp10_AttractMaxTimesToLoop variables, the TriggerAttractMode command and the CP10 AttractMode toggles.
// Rows, in constructor order (9 objects at 0x3255ce0..): 1 bool variables (caller functor tag 0x15cdf50, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: CP10 / AttractMode group with four 0x26ea0 toggles (bool, unbounded float, command, unbounded int).
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot358Rva = 0x786f50;
inline constexpr std::size_t kSlot358Size = 1769;
inline constexpr std::uint64_t kSlot358BoolFnTag = 0x15cdf50;
inline constexpr std::uint64_t kSlot358FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot358ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot358VarFlags = 1;
inline constexpr std::uint64_t kSlot358UFloatFnTag = 0x15cdf70;
inline constexpr std::size_t kSlot358UFloatCount = 1;
inline constexpr std::size_t kSlot358BoolCount = 1;
inline constexpr std::size_t kSlot358FloatCount = 0;
inline constexpr std::size_t kSlot358RowCount = 9;
inline constexpr std::size_t kSlot358StringCount = 7;
inline constexpr std::uint64_t kSlot358IntFnTag = 0x0;
inline constexpr std::size_t kSlot358IntCount = 0;
inline constexpr std::size_t kSlot358LocalIntCount = 0;
inline constexpr std::size_t kSlot358SliderCount = 0;
inline constexpr std::uint64_t kSlot358UIntFnTag = 0x15cdf90;
inline constexpr std::size_t kSlot358UIntCount = 1;
inline constexpr std::size_t kSlot358CmdCount = 1;
inline constexpr std::uint64_t kSlot358Object0Rva = 0x3255ce0;
inline constexpr std::size_t kSlot358CellCount = 0;
inline constexpr std::size_t kSlot358HolderCount = 0;
inline constexpr char kSlot358BoolNames[kSlot358BoolCount][27] = {"attractMovieEnabled"};
inline constexpr std::size_t kSlot358BoolNameLengths[kSlot358BoolCount] = {0x13};
inline constexpr std::uint8_t kSlot358BoolValues[kSlot358BoolCount] = {1};
inline constexpr char kSlot358UFloatNames[kSlot358UFloatCount][27] = {"attractMovieTime"};
inline constexpr std::size_t kSlot358UFloatNameLengths[kSlot358UFloatCount] = {0x10};
inline constexpr std::uint32_t kSlot358UFloatValueBits[kSlot358UFloatCount] = {0x41f00000};
inline constexpr std::uint8_t kSlot358UFloatEarly[kSlot358UFloatCount] = {1};
inline constexpr std::int32_t kSlot358UFloatDesc[kSlot358UFloatCount] = {-1};
inline constexpr char kSlot358UIntNames[kSlot358UIntCount][27] = {"cp10_AttractMaxTimesToLoop"};
inline constexpr std::size_t kSlot358UIntNameLengths[kSlot358UIntCount] = {0x1a};
inline constexpr std::int32_t kSlot358UIntValue[kSlot358UIntCount] = {5};
inline constexpr char kSlot358CmdNames[kSlot358CmdCount][27] = {"TriggerAttractMode"};
inline constexpr std::size_t kSlot358CmdNameLengths[kSlot358CmdCount] = {0x12};
inline constexpr std::int32_t kSlot358CmdDesc[kSlot358CmdCount] = {0};
inline constexpr std::uint64_t kSlot358CmdHandler[kSlot358CmdCount] = {0x786090};
inline constexpr char kSlot358MenuStrings[kSlot358StringCount][57] = {"Immediately trigger the attract mode from the front end.", "CP10", "AttractMode", "Enabled", "Wait Time", "Trigger", "Max Times To Loop (-1 = Infinite)"};
inline constexpr std::size_t kSlot358MenuStringLengths[kSlot358StringCount] = {0x38, 0x4, 0xb, 0x7, 0x9, 0x7, 0x21};

struct Slot358Objects {
    ConCmdBoolVar r00;   // 0x3255ce0 bool
    ConCmdRangedVar r01;   // 0x3255da8 ufloat
    ConCmdRangedVar r02;   // 0x3255e80 uint
    ConCmdCommand r03;   // 0x3255f58 cmd
    DebugMenuItem r04;   // 0x3255ff8 group
    DebugMenuItem r05;   // 0x32560a0 toge
    DebugMenuItem r06;   // 0x3256148 toge
    DebugMenuItem r07;   // 0x32561f0 toge
    DebugMenuItem r08;   // 0x3256298 toge
};

struct Slot358Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot358Result init_slot358_786f50(Slot358Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
