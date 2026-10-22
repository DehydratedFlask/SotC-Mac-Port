// 0xde0160: memset(buf, 1, 0x400) then zero 16 adjacent trailer bytes; atexit omitted.
// --dump feeds tools/lift/check_fde0160.py (binary-vs-native tie).
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_de0160.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    static_assert(sizeof(sotc::LeafDe0160) == sotc::kLeafDe0160FillBytes + sotc::kLeafDe0160TrailerBytes);
    static_assert(offsetof(sotc::LeafDe0160, trailer) == sotc::kLeafDe0160FillBytes);

    sotc::LeafDe0160 g{};
    std::memset(g.fill.data(), 0xAA, g.fill.size());
    std::memset(g.trailer.data(), 0xBB, g.trailer.size());
    sotc::init_leaf_de0160(g);

    for (std::byte b : g.fill) CHECK(b == std::byte{1});
    for (std::byte b : g.trailer) CHECK(b == std::byte{0});

    // Unguarded rewrite is idempotent from any seed.
    for (unsigned seed : {0u, 1u, 0xFFu}) {
        sotc::LeafDe0160 p{};
        std::memset(p.fill.data(), static_cast<int>(seed), p.fill.size());
        std::memset(p.trailer.data(), static_cast<int>(~seed), p.trailer.size());
        sotc::init_leaf_de0160(p);
        CHECK(std::memcmp(p.fill.data(), g.fill.data(), g.fill.size()) == 0);
        CHECK(std::memcmp(p.trailer.data(), g.trailer.data(), g.trailer.size()) == 0);
        sotc::init_leaf_de0160(p);
        CHECK(std::memcmp(p.fill.data(), g.fill.data(), g.fill.size()) == 0);
        CHECK(std::memcmp(p.trailer.data(), g.trailer.data(), g.trailer.size()) == 0);
    }

    if (argc > 1) {
        std::printf("{\"fill_bytes\":%zu,\"fill_value\":1,\"trailer_bytes\":%zu,\"trailer_value\":0}\n",
                    sotc::kLeafDe0160FillBytes, sotc::kLeafDe0160TrailerBytes);
    } else {
        std::puts("leaf_de0160: all checks passed");
    }
    return 0;
}
