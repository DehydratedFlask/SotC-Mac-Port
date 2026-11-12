// CPU-01-STARTUP-SLOT-505: native model of the two helpers slot 505 calls.
//
//   0x1621b0 (87 B): min/max accumulator over a 24-byte bounds record and a 12-byte vec3.
//   0x162970 (8 B): `lea rax, [0x2ee3ca0]; ret` -- the address of the process BSS seed record.
//
// The seed record 0x2ee3ca0 is zero-initialised BSS in the pinned image. The only constructor in
// the table that writes it is slot 526 (0x163a40), which runs after slot 505; at slot 505 the 24
// bytes are therefore all zero. A host may pass a non-zero seed through Slot505Hooks to model a
// different pre-slot state, but the production startup path uses the BSS zeroes.
//
// The min/max operations must reproduce VMINSS/VMAXSS, not std::min/std::max: if either operand is
// NaN the result is the second (source) operand; on equal operands (including +0/-0) the second
// operand is returned. The helper stores the operations in this order:
//   min.x, min.y, min.z, max.x, max.y, max.z.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kBoundsMinMax1621b0Rva = 0x1621b0;
inline constexpr std::size_t kBoundsMinMax1621b0Size = 87;
inline constexpr std::uint64_t kBoundsSeed162970Rva = 0x162970;
inline constexpr std::size_t kBoundsSeed162970Size = 8;
inline constexpr std::uint64_t kBoundsSeedCellRva = 0x2ee3ca0;
inline constexpr std::size_t kBoundsRecordSize = 0x18;

struct BoundsVec3 {
    float x;   // +0x00
    float y;   // +0x04
    float z;   // +0x08
};
static_assert(sizeof(BoundsVec3) == 12);
static_assert(offsetof(BoundsVec3, x) == 0 && offsetof(BoundsVec3, y) == 4 &&
              offsetof(BoundsVec3, z) == 8);

struct BoundsRecord {
    BoundsVec3 min;   // +0x00
    BoundsVec3 max;   // +0x0c
};
static_assert(sizeof(BoundsRecord) == kBoundsRecordSize);
static_assert(offsetof(BoundsRecord, min) == 0 && offsetof(BoundsRecord, max) == 12);

// Exact scalar VMINSS/VMAXSS semantics (NaN and signed-zero rules included).
[[nodiscard]] float bounds_minss(float current, float incoming) noexcept;
[[nodiscard]] float bounds_maxss(float current, float incoming) noexcept;

// 0x1621b0: six scalar operations in the fixed order min.x, min.y, min.z, max.x, max.y, max.z.
void bounds_minmax_1621b0(BoundsRecord& bounds, const BoundsVec3& point) noexcept;

// 0x162970: identity accessor for the 24-byte BSS cell 0x2ee3ca0. Production value is all zero
// until slot 526 writes it; this function deliberately returns a read-only zero-seed view.
[[nodiscard]] const BoundsRecord& bounds_seed_162970() noexcept;

}  // namespace sotc
