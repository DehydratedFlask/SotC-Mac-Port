// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 453 (RVA 0x48ac10, 856 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 453. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_453.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Eleven named records (0xa81700 registrar) with a group destructor 0x48a250 recorded (inference: a name table; the consumer is not established).
// Rows, in constructor order (1 objects at 0x2ef8988..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// records-only page
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

inline constexpr std::uint64_t kSlot453Rva = 0x48ac10;
inline constexpr std::size_t kSlot453Size = 856;
inline constexpr std::uint64_t kSlot453BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot453FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot453ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot453VarFlags = 1;
inline constexpr std::uint64_t kSlot453UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot453UFloatCount = 0;
inline constexpr std::size_t kSlot453BoolCount = 0;
inline constexpr std::size_t kSlot453FloatCount = 0;
inline constexpr std::size_t kSlot453RowCount = 1;
inline constexpr std::size_t kSlot453StringCount = 0;
inline constexpr std::uint64_t kSlot453IntFnTag = 0x0;
inline constexpr std::size_t kSlot453IntCount = 0;
inline constexpr std::size_t kSlot453LocalIntCount = 0;
inline constexpr std::size_t kSlot453SliderCount = 0;
inline constexpr std::uint64_t kSlot453UIntFnTag = 0x0;
inline constexpr std::size_t kSlot453UIntCount = 0;
inline constexpr std::size_t kSlot453CmdCount = 0;
inline constexpr std::size_t kSlot453StrVarCount = 0;
inline constexpr std::uint64_t kSlot453Object0Rva = 0x2ef8988;
inline constexpr std::size_t kSlot453CellCount = 0;
inline constexpr std::size_t kSlot453HolderCount = 0;
inline constexpr std::size_t kSlot453SentinelCount = 0;
inline constexpr std::size_t kSlot453RecsCount = 1;
inline constexpr std::size_t kSlot453RecordCount = 11;
inline constexpr std::uint64_t kSlot453RecsDtorRva = 0x48a250;
// Named records (0xa81700 registrar, 0x38 B stride): name, kind (edx) and flag (cl) of each record, in constructor order. Names are borrowed views; the registrar copies the view and the block is unregistered by 0x1c8990 (identity tag).
inline constexpr char kSlot453RecName[kSlot453RecordCount][40] = {"Net connection: connection count", "Net connection: connections pending", "Net connection: tx bytes (compressed)", "Net connection: rx bytes (compressed)", "Net connection: tx bytes (decompressed)", "Net connection: rx bytes (decompressed)", "Net connection: updates skipped", "Net connection: serialize time", "Net connection: deserialize time", "Net connection: compression time", "Net connection: decompression time"};
inline constexpr std::size_t kSlot453RecNameLength[kSlot453RecordCount] = {0x20, 0x23, 0x25, 0x25, 0x27, 0x27, 0x1f, 0x1e, 0x20, 0x20, 0x22};
inline constexpr std::uint32_t kSlot453RecKind[kSlot453RecordCount] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
inline constexpr std::uint8_t kSlot453RecFlag[kSlot453RecordCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr char kSlot453MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot453MenuStringLengths[1] = {0};

struct Slot453Records {
    NamedRecord rec[kSlot453RecordCount];   // consecutive 0x38 B records
};
static_assert(sizeof(Slot453Records) == 0x38 * kSlot453RecordCount);

struct Slot453Objects {
    Slot453Records r00;   // 0x2ef8988 recs
    // Host services of the named-record registrar 0xa81700 (the process registry cell and the allocator pair behind 0x17070 / 0x170b0 are NOT lifted):
    RecordRegistryCell* record_registry = nullptr;   // the registry object (the original: 0x32fd398 + guard 0x33053f0)
    AllocDispatchContract record_alloc{};            // growth of the two record-pointer vectors; see default_slot453_record_alloc()
};

struct Slot453Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot453Result init_slot453_48ac10(Slot453Objects& objects, ConCmdRegistry& concmd) noexcept;

// Host allocator stand-in (malloc / free) for the registrar vectors. NOT the engine allocator (0x12820 / 0x12af0 are not lifted); a host replaces it.
[[nodiscard]] AllocDispatchContract default_slot453_record_alloc() noexcept;

}  // namespace sotc
