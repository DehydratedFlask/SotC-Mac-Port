// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 566 (RVA 0x25de0, 1194 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 566. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_566.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): build info variables (RetailBuild, BuildVersion, ToolServerIP, MirrorWorld), the twin-B memoryToStringDisplayUnits variable with an external value cell and its Debug menu row.
// Rows, in constructor order (6 objects at 0x2dfce70..): 2 bool variables (caller functor tag 0x1595048, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 18: build info variables, one twin-B uint with an external value cell and a menu row
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
#include "startup_row_kinds.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_a81700_named_record.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"
#include "helper_140b00_twin_site.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot566Rva = 0x25de0;
inline constexpr std::size_t kSlot566Size = 1194;
inline constexpr std::uint64_t kSlot566BoolFnTag = 0x1595048;
inline constexpr std::uint64_t kSlot566FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot566ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot566VarFlags = 1;
inline constexpr std::uint64_t kSlot566UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot566UFloatCount = 0;
inline constexpr std::size_t kSlot566BoolCount = 2;
inline constexpr std::size_t kSlot566FloatCount = 0;
inline constexpr std::size_t kSlot566RowCount = 6;
inline constexpr std::size_t kSlot566StringCount = 4;
inline constexpr std::uint64_t kSlot566IntFnTag = 0x0;
inline constexpr std::size_t kSlot566IntCount = 0;
inline constexpr std::size_t kSlot566LocalIntCount = 0;
inline constexpr std::size_t kSlot566SliderCount = 0;
inline constexpr std::uint64_t kSlot566UIntTwinFinalVptr = 0x1595098;
inline constexpr std::uint64_t kSlot566UIntTwinDtorRva = 0x23d20;
inline constexpr std::uint32_t kSlot566UIntTwinSlot3Result = 1;
inline constexpr std::uint32_t kSlot566UIntTwinSlot6Result = 1;
inline constexpr std::uint64_t kSlot566UIntFnTag = 0x0;
inline constexpr std::size_t kSlot566UIntCount = 1;
inline constexpr std::size_t kSlot566CmdCount = 0;
inline constexpr std::size_t kSlot566StrVarCount = 2;
inline constexpr std::uint64_t kSlot566Object0Rva = 0x2dfce70;
inline constexpr std::size_t kSlot566CellCount = 0;
inline constexpr std::size_t kSlot566HolderCount = 0;
inline constexpr std::size_t kSlot566SentinelCount = 0;
inline constexpr char kSlot566BoolNames[kSlot566BoolCount][27] = {"RetailBuild", "MirrorWorld"};
inline constexpr std::size_t kSlot566BoolNameLengths[kSlot566BoolCount] = {0xb, 0xb};
inline constexpr std::uint8_t kSlot566BoolValues[kSlot566BoolCount] = {0, 0};
inline constexpr std::int32_t kSlot566BoolDesc[kSlot566BoolCount] = {-1, -1};
inline constexpr std::uint8_t kSlot566BoolEarly[kSlot566BoolCount] = {1, 1};
inline constexpr char kSlot566UIntNames[kSlot566UIntCount][27] = {"memoryToStringDisplayUnits"};
inline constexpr std::size_t kSlot566UIntNameLengths[kSlot566UIntCount] = {0x1a};
inline constexpr std::int32_t kSlot566UIntValue[kSlot566UIntCount] = {0};
inline constexpr std::uint8_t kSlot566UIntEarly[kSlot566UIntCount] = {1};
inline constexpr std::uint8_t kSlot566UIntTwin[kSlot566UIntCount] = {1};
inline constexpr char kSlot566StrNames[kSlot566StrVarCount][27] = {"BuildVersion", "ToolServerIP"};
inline constexpr std::size_t kSlot566StrNameLengths[kSlot566StrVarCount] = {0xc, 0xc};
inline constexpr std::int32_t kSlot566StrInit[kSlot566StrVarCount] = {0, 1};
inline constexpr char kSlot566MenuStrings[kSlot566StringCount][29] = {"LOCAL_BUILD", "127.0.0.1", "Debug", "MemoryToString Display Units"};
inline constexpr std::size_t kSlot566MenuStringLengths[kSlot566StringCount] = {0xb, 0x9, 0x5, 0x1c};

struct Slot566Objects {
    ConCmdBoolVar r00;   // 0x2dfce70 bool
    std::uint8_t gap01[0x8];
    ConCmdStringVar r01;   // 0x2dfcf40 str
    ConCmdStringVar r02;   // 0x2dfd088 str
    ConCmdBoolVar r03;   // 0x2dfd1d0 bool
    std::uint32_t cell04 = 0x0;   // 0x2dfd298: the external value dword (INPUT; default = the ELF initial image, .bss has none)
    std::uint32_t cell04_unused;   // 0x2dfd29c: never touched by this constructor
    std::uint8_t head04[0xd0];   // 0x2dfd2a0: the 0xd0 B object (model: r04 below)
    DebugMenuItem r05;   // 0x2dfd370 toge
    ConCmdRangedVar r04;   // 0x2dfd2a0 uint (twin B): relocated model of the 0xd0 B object above; value_ref = &cell04; the +0xd0 tail of the model type is not part of the original
};

struct Slot566Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot566Result init_slot566_25de0(Slot566Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
