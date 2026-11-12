// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 422 (RVA 0x56b980, 2682 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 422. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_422.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// A debug page of bool variables (one built by the out-of-line constructor 0x274730), local ranged-int wrapper variables, a command item, a node-ref row and a zero pointer slot.
// Rows, in constructor order (13 objects at 0x2f08048..): 7 bool variables (caller functor tag 0x15ca808, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// collision debug page
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
#include "helper_274730_bool_var.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot422Rva = 0x56b980;
inline constexpr std::size_t kSlot422Size = 2682;
inline constexpr std::uint64_t kSlot422BoolFnTag = 0x15ca808;
inline constexpr std::uint64_t kSlot422FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot422ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot422VarFlags = 1;
inline constexpr std::uint64_t kSlot422UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot422UFloatCount = 0;
inline constexpr std::size_t kSlot422BoolCount = 7;
inline constexpr std::size_t kSlot422FloatCount = 0;
inline constexpr std::size_t kSlot422RowCount = 13;
inline constexpr std::size_t kSlot422StringCount = 0;
inline constexpr std::uint64_t kSlot422IntFnTag = 0x0;
inline constexpr std::size_t kSlot422IntCount = 0;
inline constexpr std::size_t kSlot422LocalIntCount = 3;
inline constexpr std::size_t kSlot422SliderCount = 0;
inline constexpr std::uint64_t kSlot422UIntFnTag = 0x0;
inline constexpr std::size_t kSlot422UIntCount = 0;
inline constexpr std::size_t kSlot422CmdCount = 0;
inline constexpr std::size_t kSlot422StrVarCount = 0;
inline constexpr std::uint64_t kSlot422Object0Rva = 0x2f08048;
inline constexpr std::size_t kSlot422BoolXCount = 1;
inline constexpr std::size_t kSlot422CellCount = 0;
inline constexpr std::size_t kSlot422HolderCount = 0;
inline constexpr std::size_t kSlot422SentinelCount = 0;
inline constexpr std::size_t kSlot422ZeroPtrCount = 1;
inline constexpr std::uint64_t kSlot422ZeroPtrDtorRva = 0x565e80;
inline constexpr std::size_t kSlot422NodeRefCount = 1;
inline constexpr std::uint64_t kSlot422NodeRefDtorRva = 0x560540;
inline constexpr char kSlot422BoolNames[kSlot422BoolCount][34] = {"collision_allowReplace", "collision_createStaticCollision", "collision_enableStaticCollision", "collision_enableBPCollisionGrid", "collision_drawStats", "collision_showActiveCP3Files", "collision_enableDeferredRayCasts"};
inline constexpr std::size_t kSlot422BoolNameLengths[kSlot422BoolCount] = {0x16, 0x1f, 0x1f, 0x1f, 0x13, 0x1c, 0x20};
inline constexpr std::uint8_t kSlot422BoolValues[kSlot422BoolCount] = {1, 1, 1, 1, 0, 0, 1};
inline constexpr std::int32_t kSlot422BoolDesc[kSlot422BoolCount] = {-1, -1, -1, -1, -1, -1, -1};
inline constexpr std::uint8_t kSlot422BoolEarly[kSlot422BoolCount] = {1, 1, 1, 1, 1, 1, 1};
// Out-of-line bool variables (0x274730): the call site's name, initial byte, REAL argument-functor class tag (a bool-family table) and argument block {self, q1, q2, q3, payload, aux};
// q[0] is the block's own address (rewritten into the object by the self-reference rule), q[1] is a .text RVA read from a relocated global, q[3] a callback RVA. All identity-only.
inline constexpr BoolVar274730SiteSpec kSlot422BoolXSites[kSlot422BoolXCount] = {{"collision_debugDrawFence", 0x18, 0, 0x15ca808, {0, 0x274710, 0x0, 0x5604e0}, 0x0, 0x0}};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
// Per-site description: nullptr = the static empty string 0xf730 (never released); a literal = a hand-built borrowed view that the site RELEASES between the wrapper and the functor destroy (the 7ec160 order, release_description = true).
inline constexpr const char* kSlot422LintDesc[kSlot422LocalIntCount] = {"", "", ""};
inline constexpr std::size_t kSlot422LintDescLength[kSlot422LocalIntCount] = {0, 0, 0};
inline constexpr LocalIntSiteSpec kSlot422LocalIntSites[kSlot422LocalIntCount] = {{0x56a9b0, "collision_debugDrawFilter", 25, 0, 0, 3, 1, 0x15ca828, true}, {0x56ad70, "collision_debugDrawGroupFilter", 30, 16, 0, 16, 1, 0x15ca8c0, true}, {0x56b150, "collision_debugDrawMaterialFilter", 33, 36, 0, 36, 1, 0x15ca958, true}};
inline constexpr char kSlot422MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot422MenuStringLengths[1] = {0};

struct Slot422Objects {
    ConCmdBoolVar r00;   // 0x2f08048 bool
    ConCmdBoolVar r01;   // 0x2f08110 bool
    ConCmdBoolVar r02;   // 0x2f081d8 bool
    ConCmdBoolVar r03;   // 0x2f082a0 bool
    ConCmdBoolVar r04;   // 0x2f08368 bool
    ConCmdBoolVar r05;   // 0x2f08430 bool
    ConCmdBoolVar r06;   // 0x2f084f8 bool
    ConCmdRangedVar r07;   // 0x2f085c0 lint
    ConCmdRangedVar r08;   // 0x2f08698 lint
    ConCmdBoolVar r09;   // 0x2f08770 boolx
    ConCmdRangedVar r10;   // 0x2f08838 lint
    std::uint8_t gap11[0x18];
    NodeRef56b4b0 r11;   // 0x2f08928 nref
    std::uint64_t r12;   // 0x2f08960 zq
};

struct Slot422Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot422Result init_slot422_56b980(Slot422Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
