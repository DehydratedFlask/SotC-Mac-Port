// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 390 (RVA 0x691c50, 1035 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 390. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_390.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Legacy particle debug variables (inference from the strings): showLegacyParticle and showLegacyParticle_Overrides bools, with an empty RB-tree header, a node-reference record and three name holders around them
// Rows, in constructor order (7 objects at 0x30157c8..): 2 bool variables (caller functor tag 0x15cc670, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 14: empty tree header (host allocator input) + node-reference row + name holders + described bools
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

inline constexpr std::uint64_t kSlot390Rva = 0x691c50;
inline constexpr std::size_t kSlot390Size = 1035;
inline constexpr std::uint64_t kSlot390BoolFnTag = 0x15cc670;
inline constexpr std::uint64_t kSlot390FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot390ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot390VarFlags = 1;
inline constexpr std::uint64_t kSlot390UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot390UFloatCount = 0;
inline constexpr std::size_t kSlot390BoolCount = 2;
inline constexpr std::size_t kSlot390FloatCount = 0;
inline constexpr std::size_t kSlot390RowCount = 7;
inline constexpr std::size_t kSlot390StringCount = 3;
inline constexpr std::uint64_t kSlot390IntFnTag = 0x0;
inline constexpr std::size_t kSlot390IntCount = 0;
inline constexpr std::size_t kSlot390LocalIntCount = 0;
inline constexpr std::size_t kSlot390SliderCount = 0;
inline constexpr std::uint64_t kSlot390UIntFnTag = 0x0;
inline constexpr std::size_t kSlot390UIntCount = 0;
inline constexpr std::size_t kSlot390CmdCount = 0;
inline constexpr std::size_t kSlot390StrVarCount = 0;
inline constexpr std::uint64_t kSlot390Object0Rva = 0x30157c8;
inline constexpr std::uint64_t kSlot390HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot390HolderDtorRva = 0x8ef00;
inline constexpr std::size_t kSlot390CellCount = 0;
inline constexpr std::size_t kSlot390HolderCount = 3;
inline constexpr std::size_t kSlot390SentinelCount = 0;
inline constexpr std::size_t kSlot390TreeCount = 1;
inline constexpr std::size_t kSlot390TreeNodeSize = 0x30;
inline constexpr std::uint64_t kSlot390TreeDtorRva = 0x6918e0;
inline constexpr std::size_t kSlot390NodeRefCount = 1;
inline constexpr std::uint64_t kSlot390NodeRefDtorRva = 0x560540;
inline constexpr char kSlot390BoolNames[kSlot390BoolCount][29] = {"showLegacyParticle", "showLegacyParticle_Overrides"};
inline constexpr std::size_t kSlot390BoolNameLengths[kSlot390BoolCount] = {0x12, 0x1c};
inline constexpr std::uint8_t kSlot390BoolValues[kSlot390BoolCount] = {1, 0};
inline constexpr std::int32_t kSlot390BoolDesc[kSlot390BoolCount] = {-1, -1};
inline constexpr char kSlot390MenuStrings[kSlot390StringCount][23] = {"showParticle_Errors", "showParticle_Name", "showParticle_Transform"};
inline constexpr std::size_t kSlot390MenuStringLengths[kSlot390StringCount] = {0x13, 0x11, 0x16};

struct Slot390Objects {
    EmptyTreeHeader r00;   // 0x30157c8 tree
    NodeRef56b4b0 r01;   // 0x30157e0 nref
    NameHolder30 r02;   // 0x3015818 holder
    NameHolder30 r03;   // 0x3015848 holder
    NameHolder30 r04;   // 0x3015878 holder
    ConCmdBoolVar r05;   // 0x30158a8 bool
    ConCmdBoolVar r06;   // 0x3015970 bool
    EmptyTreeAlloc tree_alloc{};   // HOST INPUT: the allocator behind 0x17070 (not lifted); nullptr refuses the empty-tree row. See default_empty_tree_alloc().
};

struct Slot390Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot390Result init_slot390_691c50(Slot390Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
