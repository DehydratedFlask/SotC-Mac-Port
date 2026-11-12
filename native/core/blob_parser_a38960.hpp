// CPU-01-AOT-CFG-SCALE-01-BATCH25: native model of the tagged-blob reader at 0xa38960 (436 B),
// its validator 0xa39eb0 (288 B) and its accumulator 0xa39fd0 (240 B). Original ELF sha256
// d0723afb...5861d3. Everything below was read from the pinned bytes; no original RVA is executed.
//
// ---- blob encoding (inference from the three bodies, named tag/leaf/group here) -----------------
// The input is a dword stream. Each dword is `tag = (kind << 28) | count`:
//   0x0XXXXXXX  element: contributes its low 16 bits to the accumulator's `c`
//   0x1XXXXXXX  leaf:    one entry, contributes its low 16 bits to `a`
//   0x2XXXXXXX  group:   one nested run, repeated `count` times
//   0x3XXXXXXX  end-of-run marker (also a validator toggle)
//   0xbeefbeef  stream terminator
// These names are an interpretation; only the arithmetic below is claimed.
//
// ---- 0xa39eb0 (validator) ------------------------------------------------------------------
//   status = {flag, invert}; loop over tags:
//     kind 0/1:  flag &= (count != 0)
//     kind 2:    recurse with a fresh {1, 0}; flag &= (count != 0) && sub.flag
//     kind 3:    flag &= (invert ^ 1)
//     sentinel:  flag &= invert; stop
//     other:     flag = 0
//   A kind-3 tag also stops the loop. Recursion is unbounded in the original; the port caps it.
//
// ---- 0xa39fd0 (accumulator) ----------------------------------------------------------------
//   acc = {a, b, c}; loop over the same stream:
//     kind 0: c += count
//     kind 1: b += 1; a += count
//     kind 2: recurse into a zeroed sub-accumulator, then
//             b = 1 + sub.b * count + b;  a += sub.a * count;  c += count * sub.c
//     kind 3: no change, stop;  sentinel: no change, stop
//
// ---- 0xa38960 (parse) ----------------------------------------------------------------------
//   out is a 0x28-byte record:
//     +0x00 u8  valid      = validator flag (a fresh {1,1} status)
//     +0x04 u32 a          = first accumulator's (c + a)
//     +0x08 u32 size       = second accumulator's a
//     +0x0c u32 bits       = third accumulator's b
//     +0x10 u32 bytes      = (bits + 7) >> 3
//     +0x14 u32 total      = size + bytes
//     +0x18 ptr data       = 4 * dwords copy of the stream through its sentinel (valid only)
//     +0x20 ptr control    = 4-byte refcount; the first one is always released again
//   The original allocates the first refcount *before* the validity test, then, when valid, scans
//   for the sentinel, allocates 4 * (index + 1) bytes, copies (the compiler's 8/4/2/1 store chain
//   then a 0xf8f470 memcpy, which is exactly a byte copy of that length), allocates a second
//   refcount and decrements + frees the first.
//
// Host contracts (fail closed with ok == false, before the affected store):
//   * recursion depth > 64;
//   * a stream with no sentinel within 4096 dwords;
//   * an allocator hook without a matching release hook;
//   * arena exhaustion when no hooks are supplied.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kBlobValidateRva = 0xa39eb0;
inline constexpr std::size_t kBlobValidateSize = 0x120;
inline constexpr std::uint64_t kBlobAccumulateRva = 0xa39fd0;
inline constexpr std::size_t kBlobAccumulateSize = 0xf0;
inline constexpr std::uint64_t kBlobParseRva = 0xa38960;
inline constexpr std::size_t kBlobParseSize = 0x1b4;

inline constexpr std::uint32_t kBlobSentinel = 0xbeefbeef;
inline constexpr std::uint32_t kBlobKindMask = 0xf0000000;
inline constexpr std::uint32_t kBlobCountMask = 0x0000ffff;
inline constexpr std::uint32_t kBlobKindElement = 0x00000000;
inline constexpr std::uint32_t kBlobKindLeaf = 0x10000000;
inline constexpr std::uint32_t kBlobKindGroup = 0x20000000;
inline constexpr std::uint32_t kBlobKindEnd = 0x30000000;
inline constexpr unsigned kBlobMaxDepth = 64;
inline constexpr std::size_t kBlobMaxDwords = 4096;

struct BlobCursor {
    const std::uint32_t* p = nullptr;
    std::size_t remaining = 0;   // the original reads unbounded; the port refuses past this
};

struct BlobAccum {
    std::uint32_t a = 0;
    std::uint32_t b = 0;
    std::uint32_t c = 0;
};

[[nodiscard]] bool blob_validate(BlobCursor& cursor, std::uint8_t status[2],
                                 unsigned depth = 0) noexcept;
[[nodiscard]] bool blob_accumulate(BlobCursor& cursor, BlobAccum& acc,
                                   unsigned depth = 0) noexcept;

struct BlobHeader {
    std::uint8_t valid = 0;      // +0x00
    std::uint8_t gap01[3]{};     // +0x01 never written
    std::uint32_t a = 0;         // +0x04
    std::uint32_t size = 0;      // +0x08
    std::uint32_t bits = 0;      // +0x0c
    std::uint32_t bytes = 0;     // +0x10
    std::uint32_t total = 0;     // +0x14
    const std::uint32_t* data = nullptr;    // +0x18
    std::uint32_t* control = nullptr;       // +0x20
};
static_assert(sizeof(BlobHeader) == 0x28);

// 0x17090 / 0x170c0: the engine's array new/delete. Hooks: when `allocate` is set, `release`
// must be set too, otherwise the mandatory free of the first refcount cannot be modelled.
struct BlobHooks {
    void* (*allocate)(std::size_t bytes) noexcept = nullptr;
    void (*release)(void* block) noexcept = nullptr;
};

// Two refcount slots and room for 16 payload dwords, used when no hooks are supplied.
struct BlobArena {
    std::array<std::uint32_t, 2> control{};
    std::array<std::uint32_t, 16> data{};
};

struct BlobParseResult {
    bool ok = false;
    std::size_t payload_dwords = 0;
    std::size_t allocations = 0;
    bool first_refcount_released = false;
};

// `dwords` is the caller's stream length. A stream that runs past it (a group whose sub-run ends
// on the terminator instead of a kind-3 marker, or a missing terminator) makes the port refuse.
[[nodiscard]] BlobParseResult parse_blob_a38960(BlobHeader& out, const std::uint32_t* blob,
                                                std::size_t dwords, BlobArena& arena,
                                                const BlobHooks& hooks = {}) noexcept;

}  // namespace sotc
