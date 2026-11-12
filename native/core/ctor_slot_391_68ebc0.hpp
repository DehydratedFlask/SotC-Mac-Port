// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 391 (RVA 0x68ebc0, 985 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 391. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_391.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Path-data debug page (inference from the strings): gCP10ExtractPathData / gCP10BuildPathDataFromXML console commands, the shadow_pathEnableSaving bool and its Shadow Path toggle.
// Rows, in constructor order (4 objects at 0x3015518..): 1 bool variables (caller functor tag 0x15cc650, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Shadow Path / Enable Saving toggle.
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

inline constexpr std::uint64_t kSlot391Rva = 0x68ebc0;
inline constexpr std::size_t kSlot391Size = 985;
inline constexpr std::uint64_t kSlot391BoolFnTag = 0x15cc650;
inline constexpr std::uint64_t kSlot391FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot391ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot391VarFlags = 1;
inline constexpr std::uint64_t kSlot391UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot391UFloatCount = 0;
inline constexpr std::size_t kSlot391BoolCount = 1;
inline constexpr std::size_t kSlot391FloatCount = 0;
inline constexpr std::size_t kSlot391RowCount = 4;
inline constexpr std::size_t kSlot391StringCount = 4;
inline constexpr std::uint64_t kSlot391IntFnTag = 0x0;
inline constexpr std::size_t kSlot391IntCount = 0;
inline constexpr std::size_t kSlot391LocalIntCount = 0;
inline constexpr std::size_t kSlot391SliderCount = 0;
inline constexpr std::uint64_t kSlot391UIntFnTag = 0x0;
inline constexpr std::size_t kSlot391UIntCount = 0;
inline constexpr std::size_t kSlot391CmdCount = 2;
inline constexpr std::uint64_t kSlot391Object0Rva = 0x3015518;
inline constexpr std::size_t kSlot391CellCount = 0;
inline constexpr std::size_t kSlot391HolderCount = 0;
inline constexpr char kSlot391BoolNames[kSlot391BoolCount][26] = {"shadow_pathEnableSaving"};
inline constexpr std::size_t kSlot391BoolNameLengths[kSlot391BoolCount] = {0x17};
inline constexpr std::uint8_t kSlot391BoolValues[kSlot391BoolCount] = {1};
inline constexpr char kSlot391CmdNames[kSlot391CmdCount][26] = {"gCP10ExtractPathData", "gCP10BuildPathDataFromXML"};
inline constexpr std::size_t kSlot391CmdNameLengths[kSlot391CmdCount] = {0x14, 0x19};
inline constexpr std::int32_t kSlot391CmdDesc[kSlot391CmdCount] = {0, 1};
inline constexpr std::uint64_t kSlot391CmdHandler[kSlot391CmdCount] = {0x689b80, 0x689fe0};
inline constexpr char kSlot391MenuStrings[kSlot391StringCount][28] = {"Extract CP3 PathData", "Build CP3 PathData from XML", "Shadow Path", "Enable Saving"};
inline constexpr std::size_t kSlot391MenuStringLengths[kSlot391StringCount] = {0x14, 0x1b, 0xb, 0xd};

struct Slot391Objects {
    ConCmdCommand r00;   // 0x3015518 cmd
    ConCmdCommand r01;   // 0x30155b8 cmd
    ConCmdBoolVar r02;   // 0x3015658 bool
    DebugMenuItem r03;   // 0x3015720 toge
};

struct Slot391Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot391Result init_slot391_68ebc0(Slot391Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
