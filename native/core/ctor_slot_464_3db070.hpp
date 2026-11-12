// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 464 (RVA 0x3db070, 697 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 464. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_464.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// A bool variable and five named records (0xa81700 registrar) with a group destructor 0x3d9b40 recorded (inference: a name table; the consumer is not established).
// Rows, in constructor order (2 objects at 0x2ef70b0..): 1 bool variables (caller functor tag 0x15b1558, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// bool plus records
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

inline constexpr std::uint64_t kSlot464Rva = 0x3db070;
inline constexpr std::size_t kSlot464Size = 697;
inline constexpr std::uint64_t kSlot464BoolFnTag = 0x15b1558;
inline constexpr std::uint64_t kSlot464FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot464ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot464VarFlags = 1;
inline constexpr std::uint64_t kSlot464UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot464UFloatCount = 0;
inline constexpr std::size_t kSlot464BoolCount = 1;
inline constexpr std::size_t kSlot464FloatCount = 0;
inline constexpr std::size_t kSlot464RowCount = 2;
inline constexpr std::size_t kSlot464StringCount = 0;
inline constexpr std::uint64_t kSlot464IntFnTag = 0x0;
inline constexpr std::size_t kSlot464IntCount = 0;
inline constexpr std::size_t kSlot464LocalIntCount = 0;
inline constexpr std::size_t kSlot464SliderCount = 0;
inline constexpr std::uint64_t kSlot464UIntFnTag = 0x0;
inline constexpr std::size_t kSlot464UIntCount = 0;
inline constexpr std::size_t kSlot464CmdCount = 0;
inline constexpr std::size_t kSlot464StrVarCount = 0;
inline constexpr std::uint64_t kSlot464Object0Rva = 0x2ef70b0;
inline constexpr std::size_t kSlot464CellCount = 0;
inline constexpr std::size_t kSlot464HolderCount = 0;
inline constexpr std::size_t kSlot464SentinelCount = 0;
inline constexpr std::size_t kSlot464RecsCount = 1;
inline constexpr std::size_t kSlot464RecordCount = 5;
inline constexpr std::uint64_t kSlot464RecsDtorRva = 0x3d9b40;
inline constexpr char kSlot464BoolNames[kSlot464BoolCount][22] = {"network_printHiearchy"};
inline constexpr std::size_t kSlot464BoolNameLengths[kSlot464BoolCount] = {0x15};
inline constexpr std::uint8_t kSlot464BoolValues[kSlot464BoolCount] = {0};
inline constexpr std::int32_t kSlot464BoolDesc[kSlot464BoolCount] = {-1};
inline constexpr std::uint8_t kSlot464BoolEarly[kSlot464BoolCount] = {1};
// Named records (0xa81700 registrar, 0x38 B stride): name, kind (edx) and flag (cl) of each record, in constructor order. Names are borrowed views; the registrar copies the view and the block is unregistered by 0x1c8990 (identity tag).
inline constexpr char kSlot464RecName[kSlot464RecordCount][32] = {"Server: network objects", "Server: objects in view", "Server: object gather time", "Server: time to add new objects", "Server: new object count"};
inline constexpr std::size_t kSlot464RecNameLength[kSlot464RecordCount] = {0x17, 0x17, 0x1a, 0x1f, 0x18};
inline constexpr std::uint32_t kSlot464RecKind[kSlot464RecordCount] = {1, 1, 1, 1, 1};
inline constexpr std::uint8_t kSlot464RecFlag[kSlot464RecordCount] = {0, 0, 0, 0, 0};
inline constexpr char kSlot464MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot464MenuStringLengths[1] = {0};

struct Slot464Records {
    NamedRecord rec[kSlot464RecordCount];   // consecutive 0x38 B records
};
static_assert(sizeof(Slot464Records) == 0x38 * kSlot464RecordCount);

struct Slot464Objects {
    ConCmdBoolVar r00;   // 0x2ef70b0 bool
    Slot464Records r01;   // 0x2ef7178 recs
    // Host services of the named-record registrar 0xa81700 (the process registry cell and the allocator pair behind 0x17070 / 0x170b0 are NOT lifted):
    RecordRegistryCell* record_registry = nullptr;   // the registry object (the original: 0x32fd398 + guard 0x33053f0)
    AllocDispatchContract record_alloc{};            // growth of the two record-pointer vectors; see default_slot464_record_alloc()
};

struct Slot464Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot464Result init_slot464_3db070(Slot464Objects& objects, ConCmdRegistry& concmd) noexcept;

// Host allocator stand-in (malloc / free) for the registrar vectors. NOT the engine allocator (0x12820 / 0x12af0 are not lifted); a host replaces it.
[[nodiscard]] AllocDispatchContract default_slot464_record_alloc() noexcept;

}  // namespace sotc
