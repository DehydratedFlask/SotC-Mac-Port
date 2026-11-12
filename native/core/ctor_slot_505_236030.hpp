// CPU-01-STARTUP-SLOT-505: native model of startup constructor slot 505 (RVA 0x236030).
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x236030, 0x2365f4), 1476 B; body sha256
// b2d267ef8390855bbfb668cd8a8e5d7cacf60dae9e7e81d4f7c786443c9d8033. Constructor table word
// 0x2dd70e0 - 8 * 505 = 0x2dd6118 holds 0x236030.
//
// Body map (all values pinned from the original bytes; no original RVA is executed):
//
//   0x23603a..0x236052  copy 0xa90 (2704) bytes from the host source 0xfc0580 in 84 32-byte
//                       vmovups loads/stores, plus a 16-byte xmm1 tail load at 0xfc1000.
//   0x23605a            movabs rax, 0x3f5415c8be50f81a
//   0x236594            store that qword at table +0xa90. This is the final 8 bytes of the last
//                       12-byte vec3; the last vec3 is source float 675 followed by the two
//                       immediate dwords (0xbe50f81a, 0x3f5415c8).
//   0x23659b            call 0x162970 -> address of BSS record 0x2ee3ca0 (all zero at this point)
//   0x2365a0..0x2365bf  copy the 24-byte seed to bounds at table +0xa98 (qword at +0x10, xmm at +0)
//   0x2365c7..0x2365e7  for (rbx = 0; rbx != 0xa98; rbx += 0xc) call 0x1621b0(bounds,
//                       table + rbx); exactly 226 iterations.
//   0x2365e9..0x2365f3  restore the frame and return; trailing nop at 0x2365f4.
//
// There is one straight-line path and one loop branch, no stack canary, no atexit record, no
// console/menu row and no strings. Every byte of the 0xab0-byte object is written: table
// 0x2ee9820..0x2eea2b8 (0xa98 = 226 x 12) and bounds 0x2eea2b8..0x2eea2d0 (0x18). There are no
// unwritten object bytes. The source table and the seed are the only inputs.
//
// The 0x2ee3ca0 seed is BSS. Its only writer in the image is slot 526 (0x163a40), later than 505;
// the production startup path therefore copies zeroes. `Slot505Hooks::seed` can supply a non-zero
// record for tests/fixtures without changing the default behaviour. The 2,704-byte source is not
// embedded: a host supplies it from the documented asset-only import; StartupState carries a zero
// stand-in so the constructor still has a valid source in the native startup sweep.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "bounds_minmax_1621b0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot505Rva = 0x236030;
inline constexpr std::size_t kSlot505Size = 0x5c4;   // 1476 B
inline constexpr std::uint64_t kSlot505TableWordRva = 0x2dd6118;
inline constexpr std::uint64_t kSlot505SourceRva = 0xfc0580;
inline constexpr std::size_t kSlot505SourceCopySize = 0xa90;   // 2704 B
inline constexpr std::uint64_t kSlot505TableRva = 0x2ee9820;
inline constexpr std::size_t kSlot505TableSize = 0xa98;        // 226 x 12
inline constexpr std::size_t kSlot505PointCount = 226;
inline constexpr std::size_t kSlot505PointStride = 0xc;
inline constexpr std::size_t kSlot505TailOffset = 0xa90;
inline constexpr std::uint64_t kSlot505TailQword = 0x3f5415c8be50f81a;
inline constexpr std::uint64_t kSlot505BoundsRva = 0x2eea2b8;
inline constexpr std::size_t kSlot505ObjectSize = 0xab0;       // 2736 B
inline constexpr std::size_t kSlot505LoopStep = 0xc;
inline constexpr std::size_t kSlot505LoopLimit = 0xa98;
inline constexpr std::uint64_t kSlot505SeedCellRva = 0x2ee3ca0;
inline constexpr std::uint64_t kSlot505MinMaxRva = 0x1621b0;
inline constexpr std::uint64_t kSlot505SeedHelperRva = 0x162970;
inline constexpr std::uint64_t kSlot505RetRva = 0x2365f3;
inline constexpr std::uint8_t kSlot505PadByte = 0x90;

static_assert(kSlot505SourceRva + kSlot505SourceCopySize == 0xfc1010);
static_assert(kSlot505TableRva + kSlot505TableSize == kSlot505BoundsRva);
static_assert(kSlot505BoundsRva + kBoundsRecordSize ==
              kSlot505TableRva + kSlot505ObjectSize);
static_assert(kSlot505TableSize == kSlot505PointCount * kSlot505PointStride);
static_assert(kSlot505TailOffset + sizeof(std::uint64_t) == kSlot505TableSize);
static_assert(kSlot505LoopLimit == kSlot505TableSize);
static_assert(kSlot505SeedCellRva == kBoundsSeedCellRva);
static_assert(kSlot505MinMaxRva == kBoundsMinMax1621b0Rva);
static_assert(kSlot505SeedHelperRva == kBoundsSeed162970Rva);

struct Slot505Objects {
    std::array<BoundsVec3, kSlot505PointCount> points{};   // 0x2ee9820 (0xa98)
    BoundsRecord bounds{};                                 // 0x2eea2b8 (0x18)
};
static_assert(offsetof(Slot505Objects, points) == 0);
static_assert(offsetof(Slot505Objects, bounds) == kSlot505TableSize);
static_assert(sizeof(Slot505Objects) == kSlot505ObjectSize);

struct Slot505Hooks {
    // Optional non-zero seed record. Null uses bounds_seed_162970(), i.e. the BSS zeroes that are
    // actually present when index 505 runs in the constructor table.
    const BoundsRecord* seed = nullptr;
};

enum class Slot505Stage : std::uint8_t {
    none,
    source,
};
[[nodiscard]] const char* slot505_stage_name(Slot505Stage stage) noexcept;

struct Slot505Result {
    bool ok = false;
    Slot505Stage blocked = Slot505Stage::source;
    bool source_copied = false;
    bool tail_written = false;
    bool seed_copied = false;
    std::size_t points = 0;
};

// Runs constructor 505 natively. A null source is the only fail-closed condition and writes
// nothing. Otherwise the source is copied byte-for-byte, the 8-byte tail is written, the 24-byte
// seed is copied, and exactly 226 bounds updates are applied in order.
[[nodiscard]] Slot505Result init_slot505_236030(Slot505Objects& objects,
                                                const std::uint8_t* source,
                                                const Slot505Hooks& hooks = {}) noexcept;

}  // namespace sotc
