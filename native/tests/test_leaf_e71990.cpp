// The recovered table is compared with an independent oracle (records built field by field from the
// inputs and from a separately written little-endian byte table of the 14 hash immediates), several
// name-address sets, and a poisoned buffer (the 4 padding bytes after each of the first 13 records
// must stay untouched). --dump feeds
// tools/lift/check_fe71990.py (binary-vs-native tie), declaring the name addresses it used.
#include <array>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "core/leaf_e71990.hpp"
#include "core/name_hash.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::size_t kSize = 220;
constexpr std::uint8_t kPoison = 0xAA;
// The original 14 hash immediates, little-endian bytes (independent of the header's table).
constexpr std::array<std::array<std::uint8_t, 4>, 14> kHashBytes = {{
    {0x49, 0x80, 0xe5, 0xf1}, {0x23, 0xa3, 0x7d, 0x25}, {0x13, 0xc4, 0xa2, 0x47}, {0xe8, 0x6a, 0x7e, 0xed},
    {0xcd, 0x7d, 0xd4, 0xb2}, {0x1c, 0xf5, 0x41, 0x49}, {0x1d, 0xf5, 0x41, 0x49}, {0x1e, 0xf5, 0x41, 0x49},
    {0x1f, 0xf5, 0x41, 0x49}, {0x57, 0x72, 0xef, 0x10}, {0xc9, 0x59, 0xae, 0x7c}, {0x24, 0xbb, 0x0b, 0x33},
    {0xe7, 0x88, 0x14, 0x2b}, {0x00, 0x00, 0x00, 0x00},
}};

bool covered(std::size_t i) {
    const std::size_t k = i / 0x10, off = i % 0x10;
    if (k >= 14) return false;
    if (off >= 0x0c) return false;
    return true;
}

std::array<std::uint8_t, kSize> expected(const sotc::LeafE71990Inputs& in, std::uint8_t fill) {
    std::array<std::uint8_t, kSize> out;
    out.fill(fill);
    for (std::size_t k = 0; k != 14; ++k) {
        for (std::size_t b = 0; b != 8; ++b) out[0x10 * k + b] = static_cast<std::uint8_t>(in.name_address[k] >> (8 * b));
        for (std::size_t b = 0; b != 4; ++b) out[0x10 * k + 8 + b] = kHashBytes[k][b];
    }
    return out;
}

sotc::LeafE71990Inputs inputs(unsigned seed) {
    sotc::LeafE71990Inputs in{};
    for (std::size_t k = 0; k != in.name_address.size(); ++k) {
        std::uint64_t v = 0;
        for (unsigned b = 0; b != 8; ++b) v |= static_cast<std::uint64_t>(1 + ((seed * 31 + k * 17 + b * 29) % 0x7f)) << (8 * b);
        in.name_address[k] = v;
    }
    return in;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    std::size_t covered_count = 0;
    for (std::size_t i = 0; i != kSize; ++i) covered_count += covered(i);
    CHECK(covered_count == 168);
    for (std::size_t k = 0; k != 14; ++k) {
        std::uint32_t h = 0;
        std::memcpy(&h, kHashBytes[k].data(), 4);
        CHECK(h == kLeafE71990Hash[k]);
    }
    // 13 real hashes are never 0 (name_hash remaps a raw 0 to 1); the 14th record is the literal 0.
    for (std::size_t k = 0; k != 13; ++k) CHECK(kLeafE71990Hash[k] != 0);
    CHECK(kLeafE71990Hash[13] == 0);
    CHECK(name_hash(nullptr) == 0);   // 0 is the "no name" value the sentinel record also carries

    for (unsigned seed = 0; seed != 5; ++seed) {
        const auto in = inputs(seed);
        LeafE71990 g{};
        init_leaf_e71990(g, in);
        CHECK(g.cells == expected(in, 0));
    }

    // Poisoned buffer: covered bytes rewritten, padding untouched; idempotent.
    const auto in = inputs(2);
    LeafE71990 p{};
    p.cells.fill(kPoison);
    init_leaf_e71990(p, in);
    const auto want = expected(in, kPoison);
    CHECK(p.cells == want);
    for (std::size_t i = 0; i != kSize; ++i) if (!covered(i)) CHECK(p.cells[i] == kPoison);
    init_leaf_e71990(p, in);
    CHECK(p.cells == want);
    // A second call with different addresses rewrites every pointer (the hashes stay).
    const auto in2 = inputs(4);
    init_leaf_e71990(p, in2);
    CHECK(p.cells == expected(in2, kPoison));

    if (argc > 1) {
        LeafE71990 z{};
        init_leaf_e71990(z, in);
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"", 0xe71990u, 337u,
                    static_cast<unsigned long long>(0x3353f40ull));
        for (auto byte : z.cells) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        // Recompute the poisoned image for the dump input (p now holds in2).
        LeafE71990 q{};
        q.cells.fill(kPoison);
        init_leaf_e71990(q, in);
        for (auto byte : q.cells) std::printf("%02x", byte);
        std::printf("\",\"addresses\":{");
        static const unsigned long long names[14] = {0x131c990ull, 0x131c9a2ull, 0x131c9b2ull, 0x131c9c3ull, 0x131c9d5ull,
                                                     0x131c9e4ull, 0x131c9f7ull, 0x131ca0aull, 0x131ca1dull, 0x131ca30ull,
                                                     0x131ca44ull, 0x131ca59ull, 0x131ca78ull, 0x131ca94ull};
        for (std::size_t k = 0; k != 14; ++k)
            std::printf("%s\"0x%llx\":\"0x%llx\"", k ? "," : "", names[k], static_cast<unsigned long long>(in.name_address[k]));
        std::printf("}}\n");
    } else {
        std::puts("leaf_e71990: all checks passed");
    }
    return 0;
}
