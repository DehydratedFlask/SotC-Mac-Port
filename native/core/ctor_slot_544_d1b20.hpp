// CPU-01-STARTUP-SLOT-544 (lane m6): native model of startup constructor slot 544 (RVA 0xd1b20, 273 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 544. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_544.py. NOTHING here
// executes, loads or calls an original RVA; the PLT thunk 0xf8f4f0 is the `memset` import (pinned separately by
// tools/lift/check_ff8f4f0.py) and is modelled by std::memset.
//
// Shape (observation, no stack canary, no __cxa_atexit, no console objects):
//   1. memset(0x2ed3f10, 0, 0xae40)  -- a 0xae40-byte block is zeroed.
//   2. A 13-entry table of 16-byte {dword id, (+4 never written), char* name} records starting at 0x2eded90 is
//      filled with SIX groups of stores (entry k lives at 0x2eded90 + 0x10 * k):
//        k0..k3   ids 7, 8, 9, 10      names SpotShadow0 .. SpotShadow3
//        k4..k6   NOT written by this constructor (three whole records stay untouched)
//        k7..k10  ids 0xc, 0xd, 0xe, 0xf  names ShadowCascade0 .. ShadowCascade3
//        k11      id 0x10               name "ShadowCascade4(SuperClose)"
//        k12      id 0 (via 0xc1c70, a 7-byte `mov [rdi],esi; mov [rdi+8],rdx; ret`)  name EntireFrame
//      then the qword at 0x2edee60 (the first qword of record k13, a terminator word) is stored 0.
//   The names are borrowed pointers to the original rodata text (here: the literals below). The 0xae40-byte block
//   is NOT adjacent to the table (it ends at 0x2eded50; the table starts at 0x2eded90), so they are separate
//   native members and no adjacency is claimed.
// Inference (not observed): the ids look like shadow-pass identifiers; no consumer of the table is lifted here.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kSlot544Rva = 0xd1b20;
inline constexpr std::size_t kSlot544Size = 273;
inline constexpr std::uint64_t kSlot544BlockRva = 0x2ed3f10;
inline constexpr std::size_t kSlot544BlockSize = 0xae40;
inline constexpr std::uint64_t kSlot544TableRva = 0x2eded90;
inline constexpr std::uint64_t kSlot544TerminatorRva = 0x2edee60;
inline constexpr std::uint64_t kSlot544EntryWriterRva = 0xc1c70;   // pinned, never run; modelled by set_entry
inline constexpr std::size_t kSlot544TableEntries = 13;

inline constexpr char kSlot544Name_SpotShadow0[] = "SpotShadow0";
inline constexpr char kSlot544Name_SpotShadow1[] = "SpotShadow1";
inline constexpr char kSlot544Name_SpotShadow2[] = "SpotShadow2";
inline constexpr char kSlot544Name_SpotShadow3[] = "SpotShadow3";
inline constexpr char kSlot544Name_ShadowCascade0[] = "ShadowCascade0";
inline constexpr char kSlot544Name_ShadowCascade1[] = "ShadowCascade1";
inline constexpr char kSlot544Name_ShadowCascade2[] = "ShadowCascade2";
inline constexpr char kSlot544Name_ShadowCascade3[] = "ShadowCascade3";
inline constexpr char kSlot544Name_ShadowCascade4[] = "ShadowCascade4(SuperClose)";
inline constexpr char kSlot544Name_EntireFrame[] = "EntireFrame";

struct Slot544Entry {
    std::uint32_t id;          // +0x00 dword store
    std::uint8_t gap04[4];     // +0x04 never written
    const char* name;          // +0x08 qword store (borrowed rodata pointer)
};
static_assert(sizeof(Slot544Entry) == 0x10 && offsetof(Slot544Entry, name) == 8);

struct Slot544Objects {
    std::array<std::uint8_t, kSlot544BlockSize> block;   // 0x2ed3f10 (zeroed by memset)
    Slot544Entry entries[kSlot544TableEntries];          // 0x2eded90 ..
    std::uint64_t terminator;                            // 0x2edee60 (= table + 0xd0)
};
static_assert(offsetof(Slot544Objects, terminator) - offsetof(Slot544Objects, entries) == 0xd0);

struct Slot544Result {
    bool ok = false;
};

[[nodiscard]] Slot544Result init_slot544_d1b20(Slot544Objects& objects) noexcept;

}  // namespace sotc
