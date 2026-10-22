// 0x161f80 (native/core/fnv_span_161f80). Expected values are literals computed independently (FNV-1a
// reference vectors, hand-multiplied small cases), not read back from the code under test.
// --dump <seed_hex>:<len_dec>:<bytes_hex> ... feeds tools/lift/check_f161f80.py (interpreted machine code).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <span>
#include <string>
#include <vector>

#include "core/fnv_span_161f80.hpp"
#include "core/save_checksum.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::ByteSpan;

std::uint32_t run(const std::vector<std::uint8_t>& bytes, std::int64_t len, std::uint32_t seed) {
    const ByteSpan s{bytes.data(), len};
    return sotc::fnv_span_161f80(s, seed);
}

std::vector<std::uint8_t> unhex(const std::string& h) {
    std::vector<std::uint8_t> out;
    for (std::size_t i = 0; i + 1 < h.size(); i += 2) out.push_back(static_cast<std::uint8_t>(std::stoul(h.substr(i, 2), nullptr, 16)));
    return out;
}

void dump(int argc, char** argv) {
    std::printf("[");
    for (int i = 2; i < argc; ++i) {
        const std::string a = argv[i];
        const auto c1 = a.find(':'), c2 = a.find(':', c1 + 1);
        const auto seed = static_cast<std::uint32_t>(std::stoul(a.substr(0, c1), nullptr, 16));
        const auto len = static_cast<std::int64_t>(std::stoll(a.substr(c1 + 1, c2 - c1 - 1)));
        const auto bytes = unhex(a.substr(c2 + 1));
        std::printf("%s%u", i == 2 ? "" : ",", run(bytes, len, seed));
    }
    std::printf("]\n");
}
}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0) { dump(argc, argv); return 0; }

    const std::vector<std::uint8_t> a{'a'}, foobar{'f', 'o', 'o', 'b', 'a', 'r'};
    // Published FNV-1a 32-bit vectors (offset basis 0x811c9dc5).
    CHECK(run({}, 0, 0x811c9dc5u) == 0x811c9dc5u);
    CHECK(run(a, 1, 0x811c9dc5u) == 0xe40c292cu);
    CHECK(run(foobar, 6, 0x811c9dc5u) == 0xbf9cf968u);
    // One step by hand: (0x11111111 ^ 0x22) * 0x01000193 mod 2^32.
    CHECK(run({0x22}, 1, 0x11111111u) == 0x11111133u * 0x01000193u);
    // Non-positive lengths return the seed unchanged, whatever the pointer holds (signed compare).
    CHECK(run(foobar, 0, 0x12345678u) == 0x12345678u);
    CHECK(run(foobar, -1, 0x12345678u) == 0x12345678u);
    CHECK(run(foobar, INT64_MIN, 0xcafef00du) == 0xcafef00du);
    CHECK(sotc::fnv_span_161f80(ByteSpan{nullptr, 0}, 7u) == 7u);   // null data with len 0 is never dereferenced
    // Only `len` bytes are consumed: a longer buffer with the same prefix gives the prefix hash.
    const std::vector<std::uint8_t> longer{'f', 'o', 'o', 'b', 'a', 'r', 'X', 'Y'};
    CHECK(run(longer, 6, 0x811c9dc5u) == 0xbf9cf968u);
    // Zero-extended bytes (unlike the sign-extending 0x161fb0): 0x80 xors as 0x00000080.
    CHECK(run({0x80}, 1, 0u) == 0x80u * 0x01000193u);
    CHECK(run({0xff}, 1, 0u) == 0xffu * 0x01000193u);
    // Embedded NUL bytes are data, not terminators.
    CHECK(run({0x00, 0x00}, 2, 0x811c9dc5u) != run({0x00}, 1, 0x811c9dc5u));
    // Agrees with the independently written zero-extended save-checksum FNV on every length 0..40.
    std::vector<std::uint8_t> buf;
    for (int n = 0; n <= 40; ++n) {
        CHECK(run(buf, n, 0x811c9dc5u) == sotc::fnv1a32(std::as_bytes(std::span<const std::uint8_t>(buf)), 0x811c9dc5u));
        buf.push_back(static_cast<std::uint8_t>(n * 37 + 0x81));
    }
    std::puts("fnv_span_161f80: all checks passed");
    return 0;
}
