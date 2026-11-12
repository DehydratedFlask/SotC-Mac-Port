// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 513 (RVA 0x1d0770, 4050 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 513. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_513.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Game Manager debug page (inference from the strings): 13 named records (0xa81700), draw/showObject variables (unbounded int, local ranged int, ranged int, 4 bool), 6 explicit-text toggles under Game Manager, and a trailing zero-filled container whose destructor 0x1c5090 is recorded.
// Rows, in constructor order (15 objects at 0x2ee6d00..): 4 bool variables (caller functor tag 0x15991b0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Game Manager; explicit-text toggles.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
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

inline constexpr std::uint64_t kSlot513Rva = 0x1d0770;
inline constexpr std::size_t kSlot513Size = 4050;
inline constexpr std::uint64_t kSlot513BoolFnTag = 0x15991b0;
inline constexpr std::uint64_t kSlot513FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot513ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot513VarFlags = 1;
inline constexpr std::uint64_t kSlot513UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot513UFloatCount = 0;
inline constexpr std::size_t kSlot513BoolCount = 4;
inline constexpr std::size_t kSlot513FloatCount = 0;
inline constexpr std::size_t kSlot513RowCount = 15;
inline constexpr std::size_t kSlot513StringCount = 7;
inline constexpr std::uint64_t kSlot513IntFnTag = 0x15990f8;
inline constexpr std::size_t kSlot513IntCount = 1;
inline constexpr std::size_t kSlot513LocalIntCount = 1;
inline constexpr std::size_t kSlot513SliderCount = 0;
inline constexpr std::uint64_t kSlot513UIntFnTag = 0x15990f8;
inline constexpr std::size_t kSlot513UIntCount = 1;
inline constexpr std::size_t kSlot513CmdCount = 0;
inline constexpr std::size_t kSlot513StrVarCount = 0;
inline constexpr std::uint64_t kSlot513Object0Rva = 0x2ee6d00;
inline constexpr std::size_t kSlot513CellCount = 0;
inline constexpr std::size_t kSlot513HolderCount = 0;
inline constexpr std::size_t kSlot513SentinelCount = 0;
inline constexpr std::size_t kSlot513RecsCount = 1;
inline constexpr std::size_t kSlot513RecordCount = 13;
inline constexpr std::size_t kSlot513ZeroCount = 1;
inline constexpr std::uint64_t kSlot513RecsDtorRva = 0x1c8990;
inline constexpr std::uint64_t kSlot513ZeroDtorRva = 0x1c5090;
inline constexpr char kSlot513BoolNames[kSlot513BoolCount][22] = {"showRootObjects", "debugObjectGeneration", "debugDumpRender", "ctr_showLoadingCounts"};
inline constexpr std::size_t kSlot513BoolNameLengths[kSlot513BoolCount] = {0xf, 0x15, 0xf, 0x15};
inline constexpr std::uint8_t kSlot513BoolValues[kSlot513BoolCount] = {0, 0, 0, 0};
inline constexpr char kSlot513IntNames[kSlot513IntCount][22] = {"showObjectCountMin"};
inline constexpr std::size_t kSlot513IntNameLengths[kSlot513IntCount] = {0x12};
inline constexpr std::int32_t kSlot513IntMin[kSlot513IntCount] = {0};
inline constexpr std::int32_t kSlot513IntMax[kSlot513IntCount] = {100};
inline constexpr std::int32_t kSlot513IntValue[kSlot513IntCount] = {0};
inline constexpr char kSlot513UIntNames[kSlot513UIntCount][22] = {"drawGrid"};
inline constexpr std::size_t kSlot513UIntNameLengths[kSlot513UIntCount] = {0x8};
inline constexpr std::int32_t kSlot513UIntValue[kSlot513UIntCount] = {0};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
// Per-site description: nullptr = the static empty string 0xf730 (never released); a literal = a hand-built borrowed view that the site RELEASES between the wrapper and the functor destroy (the 7ec160 order, release_description = true).
inline constexpr const char* kSlot513LintDesc[kSlot513LocalIntCount] = {""};
inline constexpr std::size_t kSlot513LintDescLength[kSlot513LocalIntCount] = {0};
inline constexpr LocalIntSiteSpec kSlot513LocalIntSites[kSlot513LocalIntCount] = {{0x1cbe60, "showObjectCount", 15, 0, 0, 3, 1, 0x1599118, true}};
// Named records (0xa81700 registrar, 0x38 B stride): name, kind (edx) and flag (cl) of each record, in constructor order. Names are borrowed views; the registrar copies the view and the block is unregistered by 0x1c8990 (identity tag).
inline constexpr char kSlot513RecName[kSlot513RecordCount][40] = {"GameManager: object count", "GameManager: component count", "GameManager: object deletion list size", "GameManager: physics time", "GameManager: touch physics time", "GameManager: touch sensors time", "GameManager: build renderlist time", "GameManager: overall update time", "GameManager: render time", "GameManager: pre-think time", "GameManager: think time", "GameManager: post-think time", "GameManager: flush object deletion time"};
inline constexpr std::size_t kSlot513RecNameLength[kSlot513RecordCount] = {0x19, 0x1c, 0x26, 0x19, 0x1f, 0x1f, 0x22, 0x20, 0x18, 0x1b, 0x17, 0x1c, 0x27};
inline constexpr std::uint32_t kSlot513RecKind[kSlot513RecordCount] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
inline constexpr std::uint8_t kSlot513RecFlag[kSlot513RecordCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr char kSlot513MenuStrings[kSlot513StringCount][26] = {"Game Manager", "Show Object Count", "Show Object Count Min", "Show Root Objects", "Debug Generators", "Draw Grid", "Debug Dump Render Objects"};
inline constexpr std::size_t kSlot513MenuStringLengths[kSlot513StringCount] = {0xc, 0x11, 0x15, 0x11, 0x10, 0x9, 0x19};

struct Slot513Records {
    NamedRecord rec[kSlot513RecordCount];   // consecutive 0x38 B records
};
static_assert(sizeof(Slot513Records) == 0x38 * kSlot513RecordCount);

// Zero-filled container (0x1c5090 is its atexit destructor, an IDENTITY TAG): the original zeroes [+0x08, +0x18) and [+0x18, +0x20) and nothing else (+0x00 is never written; the object may extend beyond +0x20: INFERENCE).
struct Slot513Zero {
    std::uint64_t q00;   // never written
    std::uint64_t q08;   // = 0
    std::uint64_t q10;   // = 0
    std::uint64_t q18;   // = 0
};
static_assert(sizeof(Slot513Zero) == 0x20);

struct Slot513Objects {
    Slot513Records r00;   // 0x2ee6d00 recs
    ConCmdRangedVar r01;   // 0x2ee6fd8 uint
    ConCmdRangedVar r02;   // 0x2ee70b0 lint
    ConCmdRangedVar r03;   // 0x2ee7188 int
    ConCmdBoolVar r04;   // 0x2ee7260 bool
    ConCmdBoolVar r05;   // 0x2ee7328 bool
    ConCmdBoolVar r06;   // 0x2ee73f0 bool
    ConCmdBoolVar r07;   // 0x2ee74b8 bool
    DebugMenuItem r08;   // 0x2ee7580 toge
    DebugMenuItem r09;   // 0x2ee7628 toge
    DebugMenuItem r10;   // 0x2ee76d0 toge
    DebugMenuItem r11;   // 0x2ee7778 toge
    DebugMenuItem r12;   // 0x2ee7820 toge
    DebugMenuItem r13;   // 0x2ee78c8 toge
    Slot513Zero r14;   // 0x2ee7970 zcont
    // Host services of the named-record registrar 0xa81700 (the process registry cell and the allocator pair behind 0x17070 / 0x170b0 are NOT lifted):
    RecordRegistryCell* record_registry = nullptr;   // the registry object (the original: 0x32fd398 + guard 0x33053f0)
    AllocDispatchContract record_alloc{};            // growth of the two record-pointer vectors; see default_slot513_record_alloc()
};

struct Slot513Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot513Result init_slot513_1d0770(Slot513Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

// Host allocator stand-in (malloc / free) for the registrar vectors. NOT the engine allocator (0x12820 / 0x12af0 are not lifted); a host replaces it.
[[nodiscard]] AllocDispatchContract default_slot513_record_alloc() noexcept;

}  // namespace sotc
