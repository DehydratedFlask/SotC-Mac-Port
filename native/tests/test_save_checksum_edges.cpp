// Boundary tests for core/save_checksum (FNV-1a-32). The implementation is not changed here; this
// file only exercises edges the existing test_save_checksum.cpp does not cover.
//
// Oracles: the published FNV-1a-32 reference vectors ("a" = 0xe40c292c, "foobar" = 0xbf9cf968,
// empty = 0x811c9dc5) plus a byte-at-a-time reference loop written in this file.
//
// NOT RUN / out of scope: a span whose size equals std::numeric_limits<std::size_t>::max().
// Constructing such a span over real memory is not representable in a test process, and calling
// fnv1a32 on it would read unmapped memory; that case is therefore untested rather than asserted.
// The largest span this test actually allocates is kLargeSpan bytes (see below).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <span>
#include <vector>

#include "core/save_checksum.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

std::uint32_t fnv1a32_reference(std::span<const std::byte> bytes, std::uint32_t seed) {
    std::uint32_t hash = seed;
    for (const std::byte byte : bytes) {
        hash = (hash ^ std::to_integer<std::uint32_t>(byte)) * 0x01000193u;
    }
    return hash;
}

constexpr std::size_t kLargeSpan = 1u << 22;   // 4 MiB: many "chunks" for any chunking scheme

}  // namespace

int main() {
    using sotc::fnv1a32;
    constexpr std::uint32_t offset_basis = 0x811c9dc5u;

    // Empty span with the production seed: the seed is returned unchanged.
    const std::span<const std::byte> empty;
    CHECK(fnv1a32(empty, offset_basis) == 0x811c9dc5u);
    // An empty span at a valid (non-null) pointer must not dereference anything.
    const std::array<std::byte, 1> one_byte{std::byte{0x5a}};
    CHECK(fnv1a32(std::span<const std::byte>(one_byte.data(), 0), offset_basis) == offset_basis);

    // Single byte: published vector for "a" (0x61).
    const std::array<std::byte, 1> a{std::byte{0x61}};
    CHECK(fnv1a32(a, offset_basis) == 0xe40c292cu);
    CHECK(fnv1a32(a, offset_basis) == fnv1a32_reference(a, offset_basis));
    const std::array<std::byte, 1> zero{std::byte{0x00}};
    CHECK(fnv1a32(zero, offset_basis) == fnv1a32_reference(zero, offset_basis));
    CHECK(fnv1a32(zero, offset_basis) != fnv1a32(a, offset_basis));

    // Published multi-byte vector: "foobar".
    const std::array<std::byte, 6> foobar{std::byte{0x66}, std::byte{0x6f}, std::byte{0x6f},
                                          std::byte{0x62}, std::byte{0x61}, std::byte{0x72}};
    CHECK(fnv1a32(foobar, offset_basis) == 0xbf9cf968u);
    CHECK(fnv1a32(foobar, offset_basis) == fnv1a32_reference(foobar, offset_basis));

    // Seed sensitivity: the same bytes under two seeds must differ (FNV-1a is a bijection in the
    // seed for a fixed message, so equality would indicate the seed is being ignored).
    const std::uint32_t seed_a = fnv1a32(foobar, offset_basis);
    const std::uint32_t seed_b = fnv1a32(foobar, 0x12345678u);
    CHECK(seed_a != seed_b);
    CHECK(seed_b == fnv1a32_reference(foobar, 0x12345678u));

    // Large span: equals the independent byte-by-byte loop, and equals the running-seed
    // composition of consecutive sub-spans (so any internal chunking must be transparent).
    std::vector<std::byte> large(kLargeSpan);
    for (std::size_t i = 0; i != large.size(); ++i) {
        large[i] = static_cast<std::byte>((i * 31u + (i >> 8)) & 0xffu);
    }
    const std::span<const std::byte> large_span(large.data(), large.size());
    const std::uint32_t large_hash = fnv1a32(large_span, offset_basis);
    CHECK(large_hash == fnv1a32_reference(large_span, offset_basis));

    std::uint32_t running = offset_basis;
    constexpr std::size_t chunk = 4093;   // deliberately not a power of two
    for (std::size_t offset = 0; offset < large.size(); offset += chunk) {
        const std::size_t count = std::min(chunk, large.size() - offset);
        running = fnv1a32(std::span<const std::byte>(large.data() + offset, count), running);
    }
    CHECK(running == large_hash);

    // Maximum size_t value must not be reachable by the implementation's own arithmetic for a
    // feasible span: assert the type-level bound used above and that the largest allocated span
    // is far below it.
    CHECK(kLargeSpan < std::numeric_limits<std::size_t>::max() / 2);
    CHECK(std::numeric_limits<std::size_t>::max() > large.size());

    std::puts("save_checksum_edges: all checks passed");
    return 0;
}
