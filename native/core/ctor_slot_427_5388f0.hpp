// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 427 (RVA 0x5388f0, 4362 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 427. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_427.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): console variables, menu toggles, two single named records and a zero cell.
// Rows, in constructor order (25 objects at 0x2efe2f8..): 7 bool variables (caller functor tag 0x15c8db8, dtor 0x20b0), 1 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15c8e78, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 17: console variables, menu items, single named records and a zero cell
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

inline constexpr std::uint64_t kSlot427Rva = 0x5388f0;
inline constexpr std::size_t kSlot427Size = 4362;
inline constexpr std::uint64_t kSlot427BoolFnTag = 0x15c8db8;
inline constexpr std::uint64_t kSlot427FloatFnTag = 0x15c8e78;
inline constexpr std::uint32_t kSlot427ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot427VarFlags = 1;
inline constexpr std::uint64_t kSlot427UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot427UFloatCount = 0;
inline constexpr std::size_t kSlot427BoolCount = 7;
inline constexpr std::size_t kSlot427FloatCount = 1;
inline constexpr std::size_t kSlot427RowCount = 25;
inline constexpr std::size_t kSlot427StringCount = 12;
inline constexpr std::uint64_t kSlot427IntFnTag = 0x0;
inline constexpr std::size_t kSlot427IntCount = 0;
inline constexpr std::size_t kSlot427LocalIntCount = 0;
inline constexpr std::size_t kSlot427SliderCount = 0;
inline constexpr std::uint64_t kSlot427UIntFnTag = 0x15c8e58;
inline constexpr std::size_t kSlot427UIntCount = 1;
inline constexpr std::size_t kSlot427CmdCount = 0;
inline constexpr std::size_t kSlot427StrVarCount = 1;
inline constexpr std::uint64_t kSlot427Object0Rva = 0x2efe2f8;
inline constexpr std::size_t kSlot427BoolXCount = 2;
inline constexpr std::uint64_t kSlot427HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot427HolderDtorRva = 0x8ef00;
inline constexpr std::size_t kSlot427CellCount = 2;
inline constexpr std::size_t kSlot427HolderCount = 1;
inline constexpr std::size_t kSlot427SentinelCount = 0;
inline constexpr std::size_t kSlot427Rec1Count = 2;
inline constexpr std::uint64_t kSlot427Rec1DtorRva = 0xa81950;
inline constexpr std::size_t kSlot427ZeroCellCount = 1;
inline constexpr std::uint64_t kSlot427ZeroCellDtorRva = 0x534580;
inline constexpr char kSlot427BoolNames[kSlot427BoolCount][35] = {"showCP10HeapMemoryUsage", "show_areaInfo", "showGameDifficulty", "enableSaveIcon", "cp10_printFadeoutFOVDegrees", "cp10_enableFadeoutFOVOverride", "constantlySaveProfile"};
inline constexpr std::size_t kSlot427BoolNameLengths[kSlot427BoolCount] = {0x17, 0xd, 0x12, 0xe, 0x1b, 0x1d, 0x15};
inline constexpr std::uint8_t kSlot427BoolValues[kSlot427BoolCount] = {0, 0, 0, 0, 0, 1, 0};
inline constexpr std::int32_t kSlot427BoolDesc[kSlot427BoolCount] = {-1, -1, -1, -1, -1, -1, -1};
inline constexpr std::uint8_t kSlot427BoolEarly[kSlot427BoolCount] = {1, 1, 1, 1, 1, 1, 1};
// Out-of-line bool variables (0x274730): the call site's name, initial byte, REAL argument-functor class tag (a bool-family table) and argument block {self, q1, q2, q3, payload, aux};
// q[0] is the block's own address (rewritten into the object by the self-reference rule), q[1] is a .text RVA read from a relocated global, q[3] a callback RVA. All identity-only.
inline constexpr BoolVar274730SiteSpec kSlot427BoolXSites[kSlot427BoolXCount] = {{"lockDebugCameraToPlayerDampY", 0x1c, 0, 0x15c8db8, {0, 0x274710, 0x0, 0x534590}, 0x0, 0x0}, {"lockDebugCameraToPlayer", 0x17, 0, 0x15c8db8, {0, 0x274710, 0x0, 0x5345b0}, 0x0, 0x0}};
inline constexpr char kSlot427FloatNames[kSlot427FloatCount][35] = {"cp10_fadeoutFOVDegrees"};
inline constexpr std::size_t kSlot427FloatNameLengths[kSlot427FloatCount] = {0x16};
inline constexpr std::uint32_t kSlot427FloatLoBits[kSlot427FloatCount] = {0xbf800000};
inline constexpr std::uint32_t kSlot427FloatHiBits[kSlot427FloatCount] = {0x43340000};
inline constexpr std::uint32_t kSlot427FloatValueBits[kSlot427FloatCount] = {0x4225147b};
inline constexpr char kSlot427UIntNames[kSlot427UIntCount][35] = {"lockDebugCameraToPlayerDampYFrames"};
inline constexpr std::size_t kSlot427UIntNameLengths[kSlot427UIntCount] = {0x22};
inline constexpr std::int32_t kSlot427UIntValue[kSlot427UIntCount] = {30};
inline constexpr std::uint8_t kSlot427UIntEarly[kSlot427UIntCount] = {1};
inline constexpr char kSlot427StrNames[kSlot427StrVarCount][35] = {"quickDebugToggle"};
inline constexpr std::size_t kSlot427StrNameLengths[kSlot427StrVarCount] = {0x10};
inline constexpr std::int32_t kSlot427StrInit[kSlot427StrVarCount] = {8};
// Single named records (0xa81700 registrar, one record per object, atexit 0xa81950 = the record's own unregister): name (borrowed view), kind (edx) and flag (cl) per row.
inline constexpr char kSlot427Rec1Name[kSlot427Rec1Count][36] = {"Shadow loop: Shadow ProcessGameLoop", "Shadow loop: Shadow Game"};
inline constexpr std::size_t kSlot427Rec1NameLength[kSlot427Rec1Count] = {0x23, 0x18};
inline constexpr std::uint32_t kSlot427Rec1Kind[kSlot427Rec1Count] = {1, 1};
inline constexpr std::uint8_t kSlot427Rec1Flag[kSlot427Rec1Count] = {1, 1};
inline constexpr char kSlot427MenuStrings[kSlot427StringCount][26] = {"TimeSinceLastProfileSave", "TimeSinceLastProgressSave", "shadowDemoMode", "Memory", "MemoryMisc", "Show CP10 Heap Memory", "Debug", "Display Current Area", "CP10_SwitchToPS4Mode", "Shadow Camera", "Save System", "Constantly Save Profile"};
inline constexpr std::size_t kSlot427MenuStringLengths[kSlot427StringCount] = {0x18, 0x19, 0xe, 0x6, 0xa, 0x15, 0x5, 0x14, 0x14, 0xd, 0xb, 0x17};

// Zero cell (0x534580 is its atexit destructor, an IDENTITY TAG; the original frees the pointer at +0 and keeps the cell): the original zeroes the three qwords [+0x00, +0x18) and nothing else.
struct Slot427ZeroCell {
    std::uint64_t q00;   // = 0
    std::uint64_t q08;   // = 0
    std::uint64_t q10;   // = 0
};
static_assert(sizeof(Slot427ZeroCell) == 0x18);

struct Slot427Objects {
    NameHashCell r00;   // 0x2efe2f8 cell
    NameHashCell r01;   // 0x2efe308 cell
    NameHolder30 r02;   // 0x2efe318 holder
    ConCmdBoolVar r03;   // 0x2efe348 bool
    DebugMenuItem r04;   // 0x2efe410 group
    DebugMenuItem r05;   // 0x2efe4b8 toge
    ConCmdBoolVar r06;   // 0x2efe560 bool
    DebugMenuItem r07;   // 0x2efe628 toge
    NamedRecord r08;   // 0x2efe6d0 rec1
    NamedRecord r09;   // 0x2efe708 rec1
    ConCmdStringVar r10;   // 0x2efe740 str
    ConCmdBoolVar r11;   // 0x2efe888 bool
    Slot427ZeroCell r12;   // 0x2efe950 zcell
    std::uint8_t gap13[0x10];
    ConCmdBoolVar r13;   // 0x2efe978 boolx
    ConCmdRangedVar r14;   // 0x2efea40 uint
    std::uint8_t gap15[0x30];
    ConCmdBoolVar r15;   // 0x2efeb48 boolx
    ConCmdBoolVar r16;   // 0x2efec10 bool
    ConCmdRangedVar r17;   // 0x2efecd8 float
    ConCmdBoolVar r18;   // 0x2efedb0 bool
    ConCmdBoolVar r19;   // 0x2efee78 bool
    DebugMenuItem r20;   // 0x2efef40 tog
    DebugMenuItem r21;   // 0x2efefe8 tog
    DebugMenuItem r22;   // 0x2eff090 tog
    ConCmdBoolVar r23;   // 0x2eff138 bool
    DebugMenuItem r24;   // 0x2eff200 toge
    // Host services of the named-record registrar 0xa81700 (the process registry cell and the allocator pair behind 0x17070 / 0x170b0 are NOT lifted):
    RecordRegistryCell* record_registry = nullptr;   // the registry object (the original: 0x32fd398 + guard 0x33053f0)
    AllocDispatchContract record_alloc{};            // growth of the two record-pointer vectors; see default_slot427_record_alloc()
};

struct Slot427Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot427Result init_slot427_5388f0(Slot427Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

// Host allocator stand-in (malloc / free) for the registrar vectors. NOT the engine allocator (0x12820 / 0x12af0 are not lifted); a host replaces it.
[[nodiscard]] AllocDispatchContract default_slot427_record_alloc() noexcept;

}  // namespace sotc
