// CPU-01-AOT-CFG-SCALE-01-BATCH26 (slot 454): native model of startup constructor slot 454
// (RVA 0x483e40, 1,431 B; FDE [0x483e40, 0x4843d7)). Original ELF sha256 d0723afb...5861d3;
// constructor-table word 0x2dd70e0 - 8 * 454 = 0x2dd62b0 holds 0x483e40.
//
// ---- body (every RVA below is read from the instructions) ------------------------------------
// Stack-guarded (0x15fe518 -> [rbp-0x30], fail 0xf8f3a0) network debug page:
//   1. 0x2ef85d8 (0xd8 B): the 0x3a980 helper builds a ConCmdBase (0x20af0, flags 1) named
//      "network_LocalPlayerId" (21) with an empty description; vptr 0x1594c08 (vtable 0x1594bf8);
//      +0x60 = &value (+0xd0), +0x68 = -1 (default), +0x6c/+0x70/+0x74/+0x78 = 0, the functor
//      block +0x80..+0x9f from the stack descriptor, +0xb0 = 0, +0xc0 = 0x1595660 (functor
//      vtable), +0xc8 = 0, value +0xd0 = -1; then 0x20d60 and __cxa_atexit(0x18a60).
//      The helper makes five indirect calls through the functor vtable and two through the
//      descriptor; their targets are runtime contract, modelled as a host hook.
//   2. 0x2ef86b0 (0xc8 B): 0x20af0 "network_Interpolation" (21), vptr 0x15940b0; +0x60 = &value
//      (+0xc0), +0x68 = 1 (default), +0x69 = 0, +0x70..0x8f = 0, +0xa0 = 0, +0xb0 = 0xc2a0,
//      +0xb8 = 0, value +0xc0 = 1; 0x20d60; __cxa_atexit(0x20b0).
//   3. 0x2ef8778 (0xc8 B): the same shape with "network_ShowObjectBandwidth" (27), default 0 and
//      value 0; __cxa_atexit(0x20b0).
//   4. four 0x38-byte records at 0x2ef8840 + 0x38*i through 0xa81700 with the text views
//      0xffb0df "Client: object count" (20, kind 0), 0xffb0f4 "Client: objects created this
//      frame" (34, kind 1), 0xffb117 "Client: objects destroyed this frame" (36, kind 1) and
//      0xffb13c "Client: updates failed" (22, kind 0); each view is released through 0xf590 and
//      the array is registered once with __cxa_atexit(0x482920). 0x482920 unregisters the four
//      records at +0, +0x38, +0x70, +0xa8 through 0xa81950; it is an identity tag.
//   5. two RB trees at 0x2ef8920 and 0x2ef8938 (0x18 B each): 0x17070 allocates a 0x38-byte
//      header, the three links self-point, colour/nil = 1; __cxa_atexit(0x47e0f0).
//   6. a 0x20-byte view at 0x2ef8950 {0xffa9a8 "SServerObjectProperties" (23), 0, 0};
//      __cxa_atexit(0xf7b0).
//   7. a third tree at 0x2ef8970 with a 0x1a8-byte header; __cxa_atexit(0x482f50).
//   8. 0x15fe518 canary compare against 0xc0(%rsp); fail 0xf8f3a0.
// Eight __cxa_atexit records in body order: 0x18a60, 0x20b0, 0x20b0, 0x482920,
// 0x47e0f0, 0x47e0f0, 0xf7b0, 0x482f50.
//
// The page identity (the network debug page) is INFERENCE from the strings; every store and
// call below is read from the instructions. The 0x47e0f0 destructor reaches the 0x482a90 RB erase
// modelled in rb_tree_482a90; see that header for
// the byte-level reachability fact (0x482960's guard compares the header's left link with itself,
// so the erase call at 0x482a28 is dead in this image). The dtors, vptr/tag values and the
// 0xf7b0 view destructor are IDENTITY TAGS: recorded, never called.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "rb_tree_482a90.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot454Rva = 0x483e40;
inline constexpr std::size_t kSlot454Size = 1431;
inline constexpr std::uint64_t kSlot454TableWord = 0x2dd62b0;

inline constexpr std::uint64_t kSlot454LocalPlayer = 0x2ef85d8;   // 0xd8 B
inline constexpr std::uint64_t kSlot454LocalPlayerCell = 0x2ef86a8; // value, also +0xd0
inline constexpr std::uint64_t kSlot454LocalPlayerDefaultCell = 0x2ef8640; // +0x68
inline constexpr std::uint64_t kSlot454Interpolation = 0x2ef86b0;  // 0xc8 B
inline constexpr std::uint64_t kSlot454InterpolationValue = 0x2ef8770; // +0xc0
inline constexpr std::uint64_t kSlot454ShowBandwidth = 0x2ef8778;  // 0xc8 B
inline constexpr std::uint64_t kSlot454ShowBandwidthValue = 0x2ef8838; // +0xc0
inline constexpr std::uint64_t kSlot454Record0 = 0x2ef8840;
inline constexpr std::size_t kSlot454RecordStride = 0x38;
inline constexpr std::size_t kSlot454RecordCount = 4;
inline constexpr std::uint64_t kSlot454TreeA = 0x2ef8920;
inline constexpr std::uint64_t kSlot454TreeB = 0x2ef8938;
inline constexpr std::uint64_t kSlot454Properties = 0x2ef8950;
inline constexpr std::uint64_t kSlot454TreeC = 0x2ef8970;

inline constexpr std::uint64_t kSlot454LocalPlayerVptr = 0x1594c08;   // vtable 0x1594bf8
inline constexpr std::uint64_t kSlot454LocalPlayerVtable = 0x1594bf8;
inline constexpr std::uint64_t kSlot454LocalPlayerFunctorVptr = 0x1595660;
inline constexpr std::uint64_t kSlot454BoolVptr = 0x15940b0;          // vtable 0x15940a0
inline constexpr std::uint64_t kSlot454BoolVtable = 0x15940a0;
inline constexpr std::uint64_t kSlot454BoolFnTag = 0x15be810;         // relocation -> 0xc2a0
inline constexpr std::uint64_t kSlot454BoolFnTagValue = 0xc2a0;
inline constexpr std::uint64_t kSlot454FunctorDescriptor = 0x15be7f0;
inline constexpr std::uint32_t kSlot454Flags = 1;

inline constexpr std::uint64_t kSlot454LocalPlayerDtor = 0x18a60;
inline constexpr std::uint64_t kSlot454BoolDtor = 0x20b0;
inline constexpr std::uint64_t kSlot454RecordsDtor = 0x482920;
inline constexpr std::uint64_t kSlot454TreeDtorA = 0x47e0f0;
inline constexpr std::uint64_t kSlot454TreeDtorB = 0x482f50;
inline constexpr std::uint64_t kSlot454ViewDtor = 0xf7b0;
inline constexpr std::uint64_t kSlot454DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot454AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot454CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot454StackChkFailRva = 0xf8f3a0;

inline constexpr std::uint64_t kSlot454LocalPlayerNameRva = 0xffa960;
inline constexpr std::size_t kSlot454LocalPlayerNameLength = 0x15;   // "network_LocalPlayerId"
inline constexpr std::uint64_t kSlot454InterpolationNameRva = 0xffa976;
inline constexpr std::size_t kSlot454InterpolationNameLength = 0x15; // "network_Interpolation"
inline constexpr std::uint64_t kSlot454ShowBandwidthNameRva = 0xffa98c;
inline constexpr std::size_t kSlot454ShowBandwidthNameLength = 0x1b; // "network_ShowObjectBandwidth"
inline constexpr std::uint64_t kSlot454DescriptionRva = 0xffb16f;    // empty string
inline constexpr std::uint64_t kSlot454PropertiesRva = 0xffa9a8;
inline constexpr std::size_t kSlot454PropertiesLength = 0x17;        // "SServerObjectProperties"

inline constexpr std::uint64_t kSlot454RecordTextRva[4] = {0xffb0df, 0xffb0f4, 0xffb117, 0xffb13c};
inline constexpr std::size_t kSlot454RecordTextLength[4] = {0x14, 0x22, 0x24, 0x16};
inline constexpr std::uint32_t kSlot454RecordKind[4] = {0, 1, 1, 0};
inline constexpr const char* kSlot454RecordText[4] = {
    "Client: object count",
    "Client: objects created this frame",
    "Client: objects destroyed this frame",
    "Client: updates failed",
};

// 0x2ef85d8: ConCmdBase + the ranged-command tail. The value cell is +0xd0.
struct Slot454LocalPlayer {
    ConCmdBase base;                 // +0x00
    std::uint64_t value_ref;         // +0x60 -> &value
    std::int32_t default_value;      // +0x68 = -1
    std::uint8_t zero6c;             // +0x6c
    std::uint8_t gap6d[3];           // +0x6d never written
    std::uint8_t zero70;             // +0x70
    std::uint8_t gap71[3];           // +0x71 never written
    std::uint8_t zero74;             // +0x74
    std::uint8_t gap75[3];           // +0x75 never written
    std::uint8_t zero78;             // +0x78
    std::uint8_t gap79[3];           // +0x79 never written
    std::uint8_t gap7c[4];           // +0x7c never written
    std::uint64_t functor0;          // +0x80
    std::uint64_t functor8;          // +0x88
    std::uint64_t functor10;         // +0x90
    std::uint64_t functor18;         // +0x98
    std::uint64_t functor_region;    // +0xa0 written by the functor contract
    std::uint8_t gapA8[8];           // +0xa8 never written
    std::uint64_t zeroB0;            // +0xb0 = 0
    std::uint8_t gapB8[8];           // +0xb8 never written
    std::uint64_t functor_vptr;      // +0xc0 = 0x1595660
    std::uint64_t zeroC8;            // +0xc8 = 0
    std::int32_t value;              // +0xd0 = -1
    std::uint8_t gapD4[4];
};
static_assert(sizeof(Slot454LocalPlayer) == 0xd8 && offsetof(Slot454LocalPlayer, value_ref) == 0x60 &&
              offsetof(Slot454LocalPlayer, default_value) == 0x68 &&
              offsetof(Slot454LocalPlayer, functor_region) == 0xa0 &&
              offsetof(Slot454LocalPlayer, functor_vptr) == 0xc0 &&
              offsetof(Slot454LocalPlayer, value) == 0xd0);

// 0x2ef86b0 / 0x2ef8778: the two network bool variables.
struct Slot454BoolTail {
    std::uint64_t value_ref;         // +0x60 -> &value
    std::uint8_t default_value;      // +0x68
    std::uint8_t zero69;             // +0x69 = 0
    std::uint8_t pad6a[6];           // +0x6a never written
    std::uint8_t zero70[0x20];       // +0x70..0x8f = 0
    std::uint8_t gap90[0x10];        // +0x90 never written
    std::uint64_t zeroA0;            // +0xa0 = 0
    std::uint8_t gapA8[8];           // +0xa8 never written
    std::uint64_t tagB0;             // +0xb0 = 0xc2a0
    std::uint64_t zeroB8;            // +0xb8 = 0
    std::uint8_t value;              // +0xc0
    std::uint8_t padC1[7];
};
static_assert(sizeof(Slot454BoolTail) == 0x68);

struct Slot454BoolVar {
    ConCmdBase base;                 // +0x00
    Slot454BoolTail tail;            // +0x60
};
static_assert(sizeof(Slot454BoolVar) == 0xc8 && offsetof(Slot454BoolVar, tail) == 0x60);

// 0x2ef8840 + 0x38*i: the four "Client:" records built by 0xa81700.
struct Slot454Record {
    SharedStr text;                  // +0x00 (data, size, aux, control)
    std::uint32_t kind;              // +0x20 (edx of 0xa81700)
    std::uint8_t gap24[4];           // +0x24 never written
    std::uint64_t zero28;            // +0x28 = 0
    std::uint8_t zero30;             // +0x30 = 0
    std::uint8_t flag31;             // +0x31 = cl (0 at every row)
    std::uint8_t pad32[6];
};
static_assert(sizeof(Slot454Record) == 0x38 && offsetof(Slot454Record, kind) == 0x20 &&
              offsetof(Slot454Record, zero28) == 0x28 && offsetof(Slot454Record, flag31) == 0x31);

struct Slot454Hooks {
    RbEraseHooks rb{};
    // 0x3a980's five functor-vtable calls plus the two descriptor calls. The original targets
    // are runtime state; a host that needs them supplies this hook. Null leaves the modelled
    // stores in place and records functor_transfers = 0.
    void* functor_context = nullptr;
    bool (*functor_transfer)(void* context, Slot454LocalPlayer& object) noexcept = nullptr;
};

struct Slot454Objects {
    Slot454LocalPlayer local_player{};                 // 0x2ef85d8
    Slot454BoolVar interpolation{};                    // 0x2ef86b0
    Slot454BoolVar show_bandwidth{};                   // 0x2ef8778
    std::array<Slot454Record, kSlot454RecordCount> records{};  // 0x2ef8840
    RbEraseTree tree_a{};                                   // 0x2ef8920
    RbEraseTree tree_b{};                                   // 0x2ef8938
    SharedStr properties{};                            // 0x2ef8950
    RbEraseTree tree_c{};                                   // 0x2ef8970
};

struct Slot454Result {
    bool ok = false;
    bool canary_matched = false;      // a native function has no cookie; recorded, always true
    std::size_t registered_commands = 0;
    std::size_t records = 0;
    std::size_t trees = 0;
    std::size_t atexit_records = 0;
    std::size_t functor_transfers = 0;  // 5 when the host hook is present
};

[[nodiscard]] Slot454Result init_slot454_483e40(Slot454Objects& objects, ConCmdRegistry& registry,
                                                const Slot454Hooks& hooks = {}) noexcept;

}  // namespace sotc
