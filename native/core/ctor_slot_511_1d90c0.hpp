// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 511 (RVA 0x1d90c0, 1550 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 511. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_511.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Profile debug variables (inference from the strings): showThinkTimes / showThinkTimesThreshold / showRenderTimes / showRenderTimesThreshold unbounded ints, four Profile toggles and two empty tree headers.
// Rows, in constructor order (10 objects at 0x2ee7d98..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 15: four unbounded ints, four 0x26db0 toggles and two empty trees
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

namespace sotc {

inline constexpr std::uint64_t kSlot511Rva = 0x1d90c0;
inline constexpr std::size_t kSlot511Size = 1550;
inline constexpr std::uint64_t kSlot511BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot511FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot511ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot511VarFlags = 1;
inline constexpr std::uint64_t kSlot511UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot511UFloatCount = 0;
inline constexpr std::size_t kSlot511BoolCount = 0;
inline constexpr std::size_t kSlot511FloatCount = 0;
inline constexpr std::size_t kSlot511RowCount = 10;
inline constexpr std::size_t kSlot511StringCount = 1;
inline constexpr std::uint64_t kSlot511IntFnTag = 0x0;
inline constexpr std::size_t kSlot511IntCount = 0;
inline constexpr std::size_t kSlot511LocalIntCount = 0;
inline constexpr std::size_t kSlot511SliderCount = 0;
inline constexpr std::uint64_t kSlot511UIntFnTag = 0x15992f0;
inline constexpr std::size_t kSlot511UIntCount = 4;
inline constexpr std::size_t kSlot511CmdCount = 0;
inline constexpr std::size_t kSlot511StrVarCount = 0;
inline constexpr std::uint64_t kSlot511Object0Rva = 0x2ee7d98;
inline constexpr std::size_t kSlot511CellCount = 0;
inline constexpr std::size_t kSlot511HolderCount = 0;
inline constexpr std::size_t kSlot511SentinelCount = 0;
inline constexpr std::size_t kSlot511TreeCount = 2;
inline constexpr std::size_t kSlot511TreeNodeSize = 0x50;
inline constexpr std::uint64_t kSlot511TreeDtorRva = 0x1d6210;
inline constexpr char kSlot511UIntNames[kSlot511UIntCount][25] = {"showThinkTimes", "showThinkTimesThreshold", "showRenderTimes", "showRenderTimesThreshold"};
inline constexpr std::size_t kSlot511UIntNameLengths[kSlot511UIntCount] = {0xe, 0x17, 0xf, 0x18};
inline constexpr std::int32_t kSlot511UIntValue[kSlot511UIntCount] = {0, 0, 0, 0};
inline constexpr std::uint8_t kSlot511UIntEarly[kSlot511UIntCount] = {1, 1, 1, 1};
inline constexpr char kSlot511MenuStrings[kSlot511StringCount][8] = {"Profile"};
inline constexpr std::size_t kSlot511MenuStringLengths[kSlot511StringCount] = {0x7};

struct Slot511Objects {
    ConCmdRangedVar r00;   // 0x2ee7d98 uint
    ConCmdRangedVar r01;   // 0x2ee7e70 uint
    ConCmdRangedVar r02;   // 0x2ee7f48 uint
    ConCmdRangedVar r03;   // 0x2ee8020 uint
    DebugMenuItem r04;   // 0x2ee80f8 tog
    DebugMenuItem r05;   // 0x2ee81a0 tog
    DebugMenuItem r06;   // 0x2ee8248 tog
    DebugMenuItem r07;   // 0x2ee82f0 tog
    EmptyTreeHeader r08;   // 0x2ee8398 tree
    EmptyTreeHeader r09;   // 0x2ee83b0 tree
    EmptyTreeAlloc tree_alloc{};   // HOST INPUT: the allocator behind 0x17070 (not lifted); nullptr refuses the empty-tree row. See default_empty_tree_alloc().
};

struct Slot511Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot511Result init_slot511_1d90c0(Slot511Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
