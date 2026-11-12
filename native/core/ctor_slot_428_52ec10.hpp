// CPU-01-AOT-CFG-SCALE-01-BATCH19: native model of startup constructor slot 428 (RVA 0x52ec10, 5375 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 428. Generated from the ELF by tools/lift/startup_gen/mk_428.py (which runs the byte tracer tools/lift/slot428_trace.py)
// and tied back to it record for record by tools/lift/check_ctor_slot_428.py. NOTHING here executes, loads or calls an original RVA: the four destructor RVAs
// are IDENTITY TAGS, recorded in the atexit log and never run.
//
// Shape (a straight-line body, 49 __cxa_atexit registrations in this order; one branch only, the stack-guard epilogue):
//   A. a 0x390 B static object (0x2efd610..0x2efd9a0) written by 98 stores, then atexit(0x52e980, null). The stores are replayed from an ELF-tied record
//      table (core/store_table). INFERENCE ONLY: seven 0x80 B records of three string views and numeric fields (a region name, a CUSA title id, a
//      256-character hexadecimal string) for the four region / title pairs. The four 256-character strings look like per-title credentials: they are
//      WITHHELD from this repository (the native copy is 256 'X'; the checker ties the ELF text by SHA-256 only). Nothing reads them natively.
//   B. five 0x58 B save descriptors at 0x2efd9a0 (stride 0x58): {u32 id, never-written +4, SharedStr name view at +8, four FNV hashes at +0x28 of the
//      SAVEINFO_* names (0xa2ddc0 -> native fnv_cstr_a2ddc0), u32 flag at +0x38, 0x18 zero bytes}; atexit(0x52eaa0, record) after each.
//   C. 37 id + name entries (0xa59620, native register_id_entry_a59620) at 0x2efdb58 (stride 0x28), ids 1..37, each source a borrowed stack view released
//      through the inert 0xf590, atexit(0x52eb00, entry) after each. INFERENCE ONLY: an achievement-like table (id -> display name).
//   D. 6 availability entries (0xa42960, native register_avail_entry_a42960) at 0x2efe120 (stride 0x48), each with a formatted-name bool variable
//      "entitlement_available_<id>", two borrowed views released (b then a), atexit(0x52eb10, entry) after each. INFERENCE ONLY: add-on item ids -> names.
//   The constants, strings and ids below are the OBSERVED data; what consumes the objects is NOT established.
//
// Allocation: phases C and D allocate (pointer-vector doubling, the 0xc8 B variables and their owned names). The original goes through the engine allocator
// 0x12820 / 0x12af0 (not modelled); the host contract is Slot428Objects::alloc (default_slot428_alloc(): malloc / free, like default_slot513_record_alloc) and
// the name deallocator. A missing contract or a failed allocation is reported (ok == false) and stops the slot at that step; nothing is faked.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "concmd_base.hpp"
#include "fnv_path_cstr_162020.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_a42960_avail_entry.hpp"
#include "helper_a59620_id_entry.hpp"
#include "shared_str_ctor_f1a0.hpp"
#include "shared_str_release_f590.hpp"
#include "store_table.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot428Rva = 0x52ec10;
inline constexpr std::size_t kSlot428Size = 5375;
inline constexpr std::uint64_t kSlot428StaticsRva = 0x2efd610;
inline constexpr std::size_t kSlot428StaticsSize = 0x390;
inline constexpr std::size_t kSlot428StoreCount = 98;
inline constexpr std::size_t kSlot428StoreStringCount = 16;
inline constexpr std::uint64_t kSlot428StoreDtorTag = 0x52e980;
inline constexpr std::uint64_t kSlot428SaveRecordRva = 0x2efd9a0;
inline constexpr std::size_t kSlot428SaveRecordStride = 0x58;
inline constexpr std::size_t kSlot428SaveRecordCount = 5;
inline constexpr std::uint64_t kSlot428SaveDtorTag = 0x52eaa0;
inline constexpr std::uint64_t kSlot428IdEntryRva = 0x2efdb58;
inline constexpr std::size_t kSlot428IdEntryCount = 37;
inline constexpr std::uint64_t kSlot428IdDtorTag = 0x52eb00;
inline constexpr std::uint64_t kSlot428AvailEntryRva = 0x2efe120;
inline constexpr std::size_t kSlot428AvailEntryCount = 6;
inline constexpr std::uint64_t kSlot428AvailDtorTag = 0x52eb10;
inline constexpr std::size_t kSlot428AtexitCount = 49;
inline constexpr std::size_t kSlot428WithheldLength = 256;

struct Slot428Statics {
    alignas(8) std::uint8_t bytes[kSlot428StaticsSize];
};
static_assert(sizeof(Slot428Statics) == kSlot428StaticsSize);

// The 0x58 B save descriptor of phase B. Bytes +4..+7 and +0x3c..+0x3f are never written by the original (zero in .bss).
struct Slot428SaveRecord {
    std::uint32_t id;            // +0x00
    std::uint8_t gap04[4];       // never written
    SharedStr name;              // +0x08: borrowed view {data, size, 0, null}
    std::uint32_t hash[4];       // +0x28: fnv_cstr_a2ddc0 of the four SAVEINFO_ names
    std::uint32_t flag;          // +0x38
    std::uint8_t gap3c[4];       // never written
    std::uint8_t zero40[16];     // +0x40 written as zero
    std::uint64_t zero50;        // +0x50 written as zero
};
static_assert(sizeof(Slot428SaveRecord) == kSlot428SaveRecordStride && offsetof(Slot428SaveRecord, name) == 8 && offsetof(Slot428SaveRecord, hash) == 0x28 &&
              offsetof(Slot428SaveRecord, flag) == 0x38 && offsetof(Slot428SaveRecord, zero40) == 0x40 && offsetof(Slot428SaveRecord, zero50) == 0x50);

struct Slot428SaveSpec {
    std::uint32_t id;
    const char* name;
    std::uint64_t name_length;
    std::uint32_t flag;
    const char* hash_text[4];
};
struct Slot428IdSpec {
    std::uint32_t id;
    const char* text;
    std::uint64_t text_length;
};
struct Slot428AvailSpec {
    const char* a;
    std::uint64_t a_length;
    const char* b;
    std::uint64_t b_length;
};

// Phase A store table (lane kinds: 0 constant, 2 string pool index). The four withheld pool strings keep the original rva and length; their text is 'X'.
inline constexpr StoreTableRecord kSlot428Stores[kSlot428StoreCount] = {
    {0x52ec7d, 0x10, 8, 2, 0, 0, 0x0, 0x0},
    {0x52ec92, 0x18, 8, 0, 0, 0, 0x6, 0x0},
    {0x52ec9d, 0x20, 16, 0, 0, 0, 0x0, 0x0},
    {0x52eca5, 0x30, 4, 0, 0, 0, 0x0, 0x0},
    {0x52ecaf, 0x34, 16, 0, 0, 0, 0x70000000d, 0x7ffdfee07ffdfee},
    {0x52ecb7, 0x44, 4, 0, 0, 0, 0x0, 0x0},
    {0x52ecc1, 0x48, 4, 0, 0, 0, 0x7ffdfee, 0x0},
    {0x52eccb, 0x4c, 4, 0, 0, 0, 0x72, 0x0},
    {0x52ecd5, 0x4, 4, 0, 0, 0, 0x100072, 0x0},
    {0x52ecdf, 0x0, 4, 0, 0, 0, 0x72, 0x0},
    {0x52ece9, 0x50, 8, 2, 0, 0, 0x1, 0x0},
    {0x52ecf0, 0x58, 8, 0, 0, 0, 0x9, 0x0},
    {0x52ecfb, 0x60, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ed03, 0x70, 8, 2, 0, 0, 0x2, 0x0},
    {0x52ed0a, 0x78, 8, 0, 0, 0, 0x100, 0x0},
    {0x52ed15, 0x80, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ed1d, 0x90, 8, 2, 0, 0, 0x3, 0x0},
    {0x52ed2b, 0x98, 8, 0, 0, 0, 0x4, 0x0},
    {0x52ed36, 0xa0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ed3e, 0xb0, 4, 0, 0, 0, 0x1, 0x0},
    {0x52ed48, 0xb4, 16, 0, 0, 0, 0x70000000d, 0x90088000900880},
    {0x52ed50, 0xc4, 4, 0, 0, 0, 0x0, 0x0},
    {0x52ed5a, 0xc8, 4, 0, 0, 0, 0x900880, 0x0},
    {0x52ed64, 0xcc, 4, 0, 0, 0, 0x72, 0x0},
    {0x52ed6e, 0xd0, 8, 2, 0, 0, 0x1, 0x0},
    {0x52ed75, 0xd8, 8, 0, 0, 0, 0x9, 0x0},
    {0x52ed80, 0xe0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ed88, 0xf0, 8, 2, 0, 0, 0x2, 0x0},
    {0x52ed8f, 0xf8, 8, 0, 0, 0, 0x100, 0x0},
    {0x52ed9a, 0x100, 16, 0, 0, 0, 0x0, 0x0},
    {0x52eda8, 0x110, 8, 2, 0, 0, 0x4, 0x0},
    {0x52edb6, 0x118, 8, 0, 0, 0, 0x4, 0x0},
    {0x52edc1, 0x120, 16, 0, 0, 0, 0x0, 0x0},
    {0x52edc9, 0x130, 4, 0, 0, 0, 0x2, 0x0},
    {0x52edd3, 0x134, 4, 0, 0, 0, 0xd, 0x0},
    {0x52eddd, 0x138, 4, 0, 0, 0, 0x8, 0x0},
    {0x52ede7, 0x13c, 16, 0, 0, 0, 0x36e5762036e5762, 0x36e576200000000},
    {0x52edef, 0x14c, 4, 0, 0, 0, 0x72, 0x0},
    {0x52ee01, 0x150, 8, 2, 0, 0, 0x5, 0x0},
    {0x52ee0f, 0x158, 8, 0, 0, 0, 0x9, 0x0},
    {0x52ee1a, 0x160, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ee22, 0x170, 8, 2, 0, 0, 0x6, 0x0},
    {0x52ee30, 0x178, 8, 0, 0, 0, 0x100, 0x0},
    {0x52ee3b, 0x180, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ee43, 0x190, 8, 2, 0, 0, 0x7, 0x0},
    {0x52ee50, 0x198, 8, 0, 0, 0, 0x4, 0x0},
    {0x52ee5b, 0x1a0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ee63, 0x1b0, 16, 0, 0, 0, 0xc00000005, 0x80000000000f},
    {0x52ee73, 0x1c0, 8, 0, 0, 0, 0x8000, 0x0},
    {0x52ee7e, 0x1c8, 4, 0, 0, 0, 0x8000, 0x0},
    {0x52ee88, 0x1cc, 4, 0, 0, 0, 0x100072, 0x0},
    {0x52ee8e, 0x1d0, 8, 2, 0, 0, 0x8, 0x0},
    {0x52ee9c, 0x1d8, 8, 0, 0, 0, 0x9, 0x0},
    {0x52eea7, 0x1e0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52eeaf, 0x1f0, 8, 2, 0, 0, 0x2, 0x0},
    {0x52eeb6, 0x1f8, 8, 0, 0, 0, 0x100, 0x0},
    {0x52eec1, 0x200, 16, 0, 0, 0, 0x0, 0x0},
    {0x52eec9, 0x210, 8, 2, 0, 0, 0x9, 0x0},
    {0x52eed7, 0x218, 8, 0, 0, 0, 0x7, 0x0},
    {0x52eee2, 0x220, 16, 0, 0, 0, 0x0, 0x0},
    {0x52eeea, 0x230, 16, 0, 0, 0, 0xc00000003, 0x401008400000007},
    {0x52eef2, 0x240, 4, 0, 0, 0, 0x4010084, 0x0},
    {0x52eefc, 0x244, 4, 0, 0, 0, 0x0, 0x0},
    {0x52ef06, 0x248, 4, 0, 0, 0, 0x4010084, 0x0},
    {0x52ef10, 0x24c, 4, 0, 0, 0, 0x100072, 0x0},
    {0x52ef16, 0x250, 8, 2, 0, 0, 0xa, 0x0},
    {0x52ef24, 0x258, 8, 0, 0, 0, 0x9, 0x0},
    {0x52ef2f, 0x260, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ef37, 0x270, 8, 2, 0, 0, 0xb, 0x0},
    {0x52ef45, 0x278, 8, 0, 0, 0, 0x100, 0x0},
    {0x52ef50, 0x280, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ef58, 0x290, 8, 2, 0, 0, 0xc, 0x0},
    {0x52ef5f, 0x298, 8, 0, 0, 0, 0x8, 0x0},
    {0x52ef6a, 0x2a0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52ef72, 0x2b0, 16, 0, 0, 0, 0xc00000004, 0x800000003},
    {0x52ef7a, 0x2c0, 4, 0, 0, 0, 0x8, 0x0},
    {0x52ef84, 0x2c4, 4, 0, 0, 0, 0x0, 0x0},
    {0x52ef8e, 0x2c8, 4, 0, 0, 0, 0x8, 0x0},
    {0x52ef98, 0x2cc, 4, 0, 0, 0, 0x100072, 0x0},
    {0x52ef9e, 0x2d0, 8, 2, 0, 0, 0xd, 0x0},
    {0x52efa5, 0x2d8, 8, 0, 0, 0, 0x9, 0x0},
    {0x52efb0, 0x2e0, 16, 0, 0, 0, 0x0, 0x0},
    {0x52efb8, 0x2f0, 8, 2, 0, 0, 0xe, 0x0},
    {0x52efd0, 0x2f8, 8, 0, 0, 0, 0x100, 0x0},
    {0x52efdb, 0x300, 16, 0, 0, 0, 0x0, 0x0},
    {0x52efe8, 0x310, 8, 2, 0, 0, 0xf, 0x0},
    {0x52efef, 0x318, 8, 0, 0, 0, 0x4, 0x0},
    {0x52effa, 0x320, 16, 0, 0, 0, 0x0, 0x0},
    {0x52f002, 0x330, 4, 0, 0, 0, 0x0, 0x0},
    {0x52f00c, 0x334, 16, 0, 0, 0, 0x70000000d, 0x7ffdfee07ffdfee},
    {0x52f014, 0x344, 8, 0, 0, 0, 0x7ffdfee00000000, 0x0},
    {0x52f01b, 0x34c, 4, 0, 0, 0, 0x72, 0x0},
    {0x52f028, 0x350, 8, 2, 0, 0, 0x1, 0x0},
    {0x52f02f, 0x358, 8, 0, 0, 0, 0x9, 0x0},
    {0x52f03a, 0x360, 16, 0, 0, 0, 0x0, 0x0},
    {0x52f042, 0x370, 8, 2, 0, 0, 0x2, 0x0},
    {0x52f049, 0x378, 8, 0, 0, 0, 0x100, 0x0},
    {0x52f054, 0x380, 16, 0, 0, 0, 0x0, 0x0},
};
inline constexpr std::uint32_t kSlot428StoreStringRva[kSlot428StoreStringCount] = {0x1013d40, 0x1013d47, 0x1013d51, 0x1013e52, 0x1013e57, 0x1013e5c, 0x1013e66, 0x1013f67, 0x1013f6c, 0x1013f76, 0x1013f7e, 0x1013f88, 0x1014089, 0x1014092, 0x101409c, 0x101419d};
inline constexpr std::uint32_t kSlot428StoreStringOffset[kSlot428StoreStringCount] = {0x0, 0x7, 0x11, 0x112, 0x117, 0x11c, 0x126, 0x227, 0x22c, 0x236, 0x23e, 0x248, 0x349, 0x352, 0x35c, 0x45d};
inline constexpr char kSlot428StoreStringBlob[] =
    "GLOBAL\0"
    "CUSA08034\0"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0"
    "SCEA\0"
    "SCEE\0"
    "CUSA08809\0"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0"
    "SCEJ\0"
    "CUSA06648\0"
    "SCEAsia\0"
    "CUSA08804\0"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0"
    "SCEChina\0"
    "CUSA08805\0"
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0"
    "Demo\0";
static_assert(sizeof(kSlot428StoreStringBlob) == 1123);   // the pool plus the literal's own terminator

inline constexpr Slot428SaveSpec kSlot428SaveSpecs[kSlot428SaveRecordCount] = {
    {2, "CollectionProfile", 17, 1, {"SAVEINFO_NewCollProfile", "SAVEINFO_NewProfileIcon", "SAVEINFO_ProfileSaveIsCorrupt", "SAVEINFO_ProfileSaveRestore"}},
    {13, "ShadowProfile", 13, 1, {"SAVEINFO_NewGameProfile", "SAVEINFO_NewProfileIcon", "SAVEINFO_ProfileSaveIsCorrupt", "SAVEINFO_ProfileSaveRestore"}},
    {4, "ShadowProgress", 14, 339, {"SAVEINFO_NewGameProgress", "SAVEINFO_NewProgressIcon", "SAVEINFO_ProgressSaveIsCorrupt", "SAVEINFO_ProgressSaveRestore"}},
    {2, "IcoProfile", 10, 1, {"SAVEINFO_NewGameProfile", "SAVEINFO_NewProfileIcon", "SAVEINFO_ProfileSaveIsCorrupt", "SAVEINFO_ProfileSaveRestore"}},
    {2, "IcoProgress", 11, 339, {"SAVEINFO_NewGameProgress", "SAVEINFO_NewProgressIcon", "SAVEINFO_ProgressSaveIsCorrupt", "SAVEINFO_ProgressSaveRestore"}},
};
inline constexpr Slot428IdSpec kSlot428IdSpecs[kSlot428IdEntryCount] = {
    {1, "Speed King", 10},
    {2, "Intrepid Mortal", 15},
    {3, "Speed Demon", 11},
    {4, "Grounded Scaler", 15},
    {5, "Resist the Wrist", 16},
    {6, "Reach the Gate", 14},
    {7, "Last Man Standing", 17},
    {8, "Bearer of the Curse", 19},
    {9, "Dormin's Rage", 13},
    {10, "Seeking Salvation", 17},
    {11, "Trick Rider", 11},
    {12, "Fruit of the Garden", 19},
    {13, "SwordOfHerMajesty", 17},
    {14, "Paint the Target", 16},
    {15, "Fruit of the Land", 17},
    {16, "Skilled Warrior", 15},
    {17, "The Past that Defines Thee", 26},
    {18, "Five-Lined Skink", 16},
    {19, "Animals of the Land", 19},
    {20, "Boon of the Nomad", 17},
    {21, "Trick Arrow Skills", 18},
    {22, "Kill Colossus 1", 15},
    {23, "Kill Colossus 2", 15},
    {24, "Kill Colossus 3", 15},
    {25, "Kill Colossus 4", 15},
    {26, "Kill Colossus 5", 15},
    {27, "Kill Colossus 6", 15},
    {28, "Kill Colossus 7", 15},
    {29, "Kill Colossus 8", 15},
    {30, "Kill Colossus 9", 15},
    {31, "Kill Colossus 10", 16},
    {32, "Kill Colossus 11", 16},
    {33, "Kill Colossus 12", 16},
    {34, "Kill Colossus 13", 16},
    {35, "Kill Colossus 14", 16},
    {36, "Kill Colossus 15", 16},
    {37, "Kill Colossus 16", 16},
};
inline constexpr Slot428AvailSpec kSlot428AvailSpecs[kSlot428AvailEntryCount] = {
    {"ANCIENTBOW000001", 16, "Ancient Bow", 11},
    {"CLOAKOFFATE00001", 16, "Cloak of Fate", 13},
    {"KNABSTRUPSTEED01", 16, "Knabstrup Steed", 15},
    {"LIFESWORD0000001", 16, "Life Sword", 10},
    {"RACKINGSTEED0001", 16, "Racking Steed", 13},
    {"SPOTTEDSTEED0001", 16, "Spotted Steed", 13},
};

struct Slot428Objects {
    Slot428Statics statics;                         // 0x2efd610
    Slot428SaveRecord saves[kSlot428SaveRecordCount];   // 0x2efd9a0
    IdEntry ids[kSlot428IdEntryCount];              // 0x2efdb58
    AvailEntry avails[kSlot428AvailEntryCount];     // 0x2efe120
    PtrVec24 id_vec;                                // V1 {0x32f8dc8, 0x32f8dd0, 0x32f8dd8}
    PtrVec24 avail_vec;                             // V2 {0x32f6f38, 0x32f6f40, 0x32f6f48}
    AllocDispatchContract alloc;                    // host stand-in for 0x12820 / 0x12af0 (vector growth, 0xc8 B variables, owned names)
    SharedStrDeallocatorF590* name_dealloc;         // host stand-in for the name release of the variables
};

struct Slot428Result {
    bool ok = false;
    bool statics_applied = false;
    std::size_t saves_done = 0;
    std::size_t ids_done = 0;
    std::size_t avails_done = 0;
    std::size_t atexit_recorded = 0;
};

// malloc / free stand-ins (ignore the tags and flags), like default_slot513_record_alloc(); the default deallocator frees with std::free.
[[nodiscard]] AllocDispatchContract default_slot428_alloc() noexcept;
[[nodiscard]] SharedStrDeallocatorF590* default_slot428_name_dealloc() noexcept;

[[nodiscard]] Slot428Result init_slot428_52ec10(Slot428Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
