// CPU-01-AOT-CFG-SCALE-01-BATCH25 (slot 495): native model of startup constructor slot 495
// (RVA 0x268ee0, 1,698 B; FDE [0x268ee0, 0x269582)). Original ELF sha256 d0723afb...5861d3;
// constructor-table word 0x2dd70e0 - 8 * 495 = 0x2dd6168 holds 0x268ee0.
//
// ---- body -----------------------------------------------------------------------------------
// A stack-guarded (0x15fe518 -> [rbp-0x30], fail 0xf8f3a0) debug-page constructor:
//   1. command object 0x2eeebb0 (0xc0 B): ConCmdBase ctor 0x20af0 with flags = 1 and the name
//      "network_AllowCrossPlatformPlay" (30) plus an empty description (0xfc4b40, length 0);
//      vptr 0x15940b0 (vtable 0x15940a0, slot 2 = 0x20d60); +0x60 = 0x2eeec70; +0x69 = 0;
//      +0x70..0x90 zero; +0xa0 = 0; +0xb0 = 0x159c9a8 (relocation 0xc2a0); +0xb8 = 0;
//      f590(description); 0x20d60(object); f590(name); __cxa_atexit(0x20b0).
//   2. parsed blob record 0x2eeeca8 (0x28 B): 0xa38960(record, 0x1604934), then
//      __cxa_atexit(0x209d80). The blob is the two dwords {0x10000020, 0xbeefbeef} (see
//      core/blob_parser_a38960); the resulting record is valid = 1, a = 0x20, size = 0x20,
//      bits = 2, bytes = 1, total = 0x21, and the payload copy is those same two dwords.
//   3. twelve 0x48-byte item records at 0x2eeecd8 + 0x48*i, each {text pointer, length, 0, 0}
//      followed by zeros at +0x20, +0x30, +0x38, +0x40 (+0x28..+0x30 is never written), a
//      f590 release of the staging view and __cxa_atexit(0x268e20).
// Fourteen __cxa_atexit records in total: 0x20b0, 0x209d80, then 0x268e20 x 12.
//
// The item texts are NET_ERROR_* names; that this page is a network-error reference is read from
// those strings and is INFERENCE. The three destructors, the +0xb0 function pointer and the vtable
// slots are IDENTITY TAGS: recorded, never called.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "blob_parser_a38960.hpp"
#include "concmd_base.hpp"
#include "ctor_slot_family_items.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot495Rva = 0x268ee0;
inline constexpr std::size_t kSlot495Size = 1698;
inline constexpr std::uint64_t kSlot495TableWord = 0x2dd6168;

inline constexpr std::uint64_t kSlot495Command = 0x2eeebb0;   // 0xc0 B
inline constexpr std::uint64_t kSlot495Region = 0x2eeec70;    // +0x60 target
inline constexpr std::uint64_t kSlot495BlobRecord = 0x2eeeca8;
inline constexpr std::uint64_t kSlot495Item0 = 0x2eeecd8;
inline constexpr std::size_t kSlot495ItemCount = 12;
inline constexpr std::size_t kSlot495ItemStride = 0x48;
inline constexpr std::size_t kSlot495CommandSize = 0xc0;

inline constexpr std::uint64_t kSlot495Vtable = 0x15940a0;
inline constexpr std::uint64_t kSlot495Vptr = 0x15940b0;         // address point
inline constexpr std::uint64_t kSlot495PreVptr = 0x1594218;      // transient store before 0x20af0
inline constexpr std::uint64_t kSlot495TagB0 = 0x159c9a8;        // relocation target 0xc2a0
inline constexpr std::uint64_t kSlot495TagB0Value = 0xc2a0;
inline constexpr std::uint32_t kSlot495Flags = 1;

inline constexpr std::uint64_t kSlot495CommandDtor = 0x20b0;
inline constexpr std::uint64_t kSlot495BlobDtor = 0x209d80;
inline constexpr std::uint64_t kSlot495ItemDtor = 0x268e20;
inline constexpr std::uint64_t kSlot495DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot495AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot495CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot495StackChkFailRva = 0xf8f3a0;

inline constexpr std::uint64_t kSlot495NameRva = 0xfc4960;
inline constexpr std::size_t kSlot495NameLength = 30;            // "network_AllowCrossPlatformPlay"
inline constexpr std::uint64_t kSlot495DescriptionRva = 0xfc4b40; // empty string
inline constexpr std::size_t kSlot495DescriptionLength = 0;
inline constexpr std::uint64_t kSlot495BlobRva = 0x1604934;
inline constexpr std::uint32_t kSlot495BlobDwords[2] = {0x10000020, 0xbeefbeef};
inline constexpr std::size_t kSlot495BlobDwordCount = 2;

// The four derived words the parse produces for the slot's static blob.
inline constexpr std::uint32_t kSlot495BlobA = 0x20;
inline constexpr std::uint32_t kSlot495BlobSize = 0x20;
inline constexpr std::uint32_t kSlot495BlobBits = 2;
inline constexpr std::uint32_t kSlot495BlobBytes = 1;
inline constexpr std::uint32_t kSlot495BlobTotal = 0x21;

struct Slot495Item {
    SharedStr text;             // +0x00
    std::uint64_t zero20;       // +0x20
    std::uint8_t gap28[8];      // +0x28 never written
    std::uint64_t zero30;       // +0x30
    std::uint64_t zero38;       // +0x38
    std::uint64_t zero40;       // +0x40
};
static_assert(sizeof(Slot495Item) == kSlot495ItemStride && offsetof(Slot495Item, zero20) == 0x20 &&
              offsetof(Slot495Item, gap28) == 0x28 && offsetof(Slot495Item, zero30) == 0x30);

struct Slot495CommandTail {
    std::uint64_t region;       // +0x60 == 0x2eeec70
    std::uint8_t pad68;         // +0x68 never written
    std::uint8_t flag69;        // +0x69 = 0
    std::uint8_t pad6a[6];      // +0x6a never written
    std::uint8_t zero70[0x20];  // +0x70..0x90 zeroed
    std::uint8_t gap90[0x10];   // +0x90 never written
    std::uint64_t zeroA0;       // +0xa0 = 0
    std::uint8_t gapA8[8];      // +0xa8 never written
    std::uint64_t tagB0;        // +0xb0 == 0xc2a0
    std::uint64_t zeroB8;       // +0xb8 = 0
};
static_assert(sizeof(Slot495CommandTail) == 0x60);

struct Slot495Command {
    ConCmdBase base;            // +0x00
    Slot495CommandTail tail;    // +0x60
};
static_assert(sizeof(Slot495Command) == kSlot495CommandSize &&
              offsetof(Slot495Command, tail) == 0x60);

// One observed item: object, text RVA, length, text.
struct Slot495ItemSpec {
    std::uint64_t object_rva;
    std::uint64_t text_rva;
    std::size_t length;
    const char* text;
};

inline constexpr std::array<Slot495ItemSpec, kSlot495ItemCount> kSlot495Items{{
    {0x2eeecd8, 0xfc497f, 35, "NET_ERROR_CONNECTIONMALFORMEDPACKET"},
    {0x2eeed20, 0xfc49a3, 37, "NET_ERROR_CONNECTIONGAMESTATEOVERFLOW"},
    {0x2eeed68, 0xfc49c9, 36, "NET_ERROR_CONNECTIONCOMPRESSIONERROR"},
    {0x2eeedb0, 0xfc49ee, 38, "NET_ERROR_CONNECTIONDECOMPRESSIONERROR"},
    {0x2eeedf8, 0xfc4a15, 31, "NET_ERROR_CONNECTIONSOCKETERROR"},
    {0x2eeee40, 0xfc4a35, 27, "NET_ERROR_CONNECTIONTIMEOUT"},
    {0x2eeee88, 0xfc4a51, 37, "NET_ERROR_CONNECTIONLOGGEDINELSEWHERE"},
    {0x2eeeed0, 0xfc4a77, 29, "NET_ERROR_CONNECTIONAUTOCLOSE"},
    {0x2eeef18, 0xfc4a95, 40, "NET_ERROR_CONNECTIONAUTHORIZATIONFAILURE"},
    {0x2eeef60, 0xfc4abe, 42, "NET_ERROR_NETWORKSCENESERIALIZATIONFAILURE"},
    {0x2eeefa8, 0xfc4ae9, 44, "NET_ERROR_NETWORKSCENEDESERIALIZATIONFAILURE"},
    {0x2eeeff0, 0xfc4b16, 30, "NET_ERROR_NETWORKSCENESHUTDOWN"},
}};

struct Slot495Objects {
    Slot495Command command{};                 // 0x2eeebb0
    std::uint8_t region60[0x38]{};            // 0x2eeec70 (only the pointer target is modelled)
    BlobHeader blob{};                        // 0x2eeeca8
    std::uint8_t gap_blob[8]{};               // 0x2eeecd0 never written
    std::array<Slot495Item, kSlot495ItemCount> items{};   // 0x2eeecd8
    BlobArena arena{};
};

struct Slot495Result {
    bool ok = false;
    bool canary_matched = false;      // a native function has no cookie; recorded, always true
    bool blob_parsed = false;
    std::size_t items = 0;
    std::size_t atexit_records = 0;
};

[[nodiscard]] Slot495Result init_slot495_268ee0(Slot495Objects& objects, ConCmdRegistry& registry,
                                                const BlobHooks& blob_hooks = {}) noexcept;

}  // namespace sotc
