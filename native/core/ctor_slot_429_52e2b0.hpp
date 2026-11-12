// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 429 (RVA 0x52e2b0, 1734 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 429. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_429.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): the Stat System page: stats_clear and stats_dump commands, the Stat On Screen bool, two groups and two command items, one zero pointer slot and three zero-header vectors.
// Rows, in constructor order (12 objects at 0x2efcd50..): 1 bool variables (caller functor tag 0x15c8470, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 19: stat system commands, a bool, groups, command items, a zero pointer slot and three zero-header vectors
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

inline constexpr std::uint64_t kSlot429Rva = 0x52e2b0;
inline constexpr std::size_t kSlot429Size = 1734;
inline constexpr std::uint64_t kSlot429BoolFnTag = 0x15c8470;
inline constexpr std::uint64_t kSlot429FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot429ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot429VarFlags = 1;
inline constexpr std::uint64_t kSlot429UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot429UFloatCount = 0;
inline constexpr std::size_t kSlot429BoolCount = 1;
inline constexpr std::size_t kSlot429FloatCount = 0;
inline constexpr std::size_t kSlot429RowCount = 12;
inline constexpr std::size_t kSlot429StringCount = 10;
inline constexpr std::uint64_t kSlot429IntFnTag = 0x0;
inline constexpr std::size_t kSlot429IntCount = 0;
inline constexpr std::size_t kSlot429LocalIntCount = 0;
inline constexpr std::size_t kSlot429SliderCount = 0;
inline constexpr std::uint64_t kSlot429UIntFnTag = 0x0;
inline constexpr std::size_t kSlot429UIntCount = 0;
inline constexpr std::size_t kSlot429CmdCount = 2;
inline constexpr std::size_t kSlot429StrVarCount = 0;
inline constexpr std::uint64_t kSlot429Object0Rva = 0x2efcd50;
inline constexpr std::size_t kSlot429CellCount = 0;
inline constexpr std::size_t kSlot429HolderCount = 0;
inline constexpr std::size_t kSlot429SentinelCount = 0;
inline constexpr std::size_t kSlot429ZVecCount = 3;
inline constexpr std::size_t kSlot429ZeroPtrCount = 1;
inline constexpr std::uint64_t kSlot429ZeroPtrDtorRva = 0x52a010;
inline constexpr char kSlot429BoolNames[kSlot429BoolCount][15] = {"showStatValues"};
inline constexpr std::size_t kSlot429BoolNameLengths[kSlot429BoolCount] = {0xe};
inline constexpr std::uint8_t kSlot429BoolValues[kSlot429BoolCount] = {0};
inline constexpr std::int32_t kSlot429BoolDesc[kSlot429BoolCount] = {-1};
inline constexpr std::uint8_t kSlot429BoolEarly[kSlot429BoolCount] = {1};
inline constexpr char kSlot429CmdNames[kSlot429CmdCount][15] = {"stats_clear", "stats_dump"};
inline constexpr std::size_t kSlot429CmdNameLengths[kSlot429CmdCount] = {0xb, 0xa};
inline constexpr std::int32_t kSlot429CmdDesc[kSlot429CmdCount] = {8, 9};
inline constexpr std::uint64_t kSlot429CmdHandler[kSlot429CmdCount] = {0x52ceb0, 0x52cec0};
inline constexpr std::uint64_t kSlot429ZVecDtorRva[kSlot429ZVecCount] = {0x52a0d0, 0x52a140, 0x52a1b0};
inline constexpr char kSlot429MenuStrings[kSlot429StringCount][24] = {"Stat System", "Clear All", "stats_clear", "Dump to Log", "stats_dump", "Stat Values", "Stat On Screen", "Stats Visible On Screen", "Clear all Stats", "Dump Stats to log"};
inline constexpr std::size_t kSlot429MenuStringLengths[kSlot429StringCount] = {0xb, 0x9, 0xb, 0xb, 0xa, 0xb, 0xe, 0x17, 0xf, 0x11};

// Zero-filled container (0x1c5090 is its atexit destructor, an IDENTITY TAG): the original zeroes [+0x08, +0x18) and [+0x18, +0x20) and nothing else (+0x00 is never written; the object may extend beyond +0x20: INFERENCE).
struct Slot429Zero {
    std::uint64_t q00;   // never written
    std::uint64_t q08;   // = 0
    std::uint64_t q10;   // = 0
    std::uint64_t q18;   // = 0
};
static_assert(sizeof(Slot429Zero) == 0x20);

struct Slot429Objects {
    ConCmdBoolVar r00;   // 0x2efcd50 bool
    DebugMenuItem r01;   // 0x2efce18 cmdi
    DebugMenuItem r02;   // 0x2efcec0 cmdi
    DebugMenuItem r03;   // 0x2efcf68 group
    DebugMenuItem r04;   // 0x2efd010 group
    DebugMenuItem r05;   // 0x2efd0b8 toge
    std::uint64_t r06;   // 0x2efd160 zq
    std::uint8_t gap07[0x8];
    Slot429Zero r07;   // 0x2efd170 zvec
    Slot429Zero r08;   // 0x2efd190 zvec
    Slot429Zero r09;   // 0x2efd1b0 zvec
    std::uint8_t gap10[0x40];
    ConCmdCommand r10;   // 0x2efd210 cmd
    ConCmdCommand r11;   // 0x2efd2b0 cmd
    const void* external_target_0 = nullptr;   // identity of the object at 0x2dfcbd0 (built by another slot; the host supplies it, the model only stores the pointer)
};

struct Slot429Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot429Result init_slot429_52e2b0(Slot429Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
