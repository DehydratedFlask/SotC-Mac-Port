// The 20-byte copy is compared with an independent byte-wise oracle for several source patterns,
// with a poisoned destination (only the 20 destination bytes may change, never the source), and
// with the aliasing case (source == destination). --dump feeds tools/lift/check_f575d40.py
// (binary-vs-native tie), declaring the run-time source bytes the call was given.
#include <array>
#include <cstdio>
#include <cstdlib>

#include "core/leaf_575d40.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::uint8_t kPoison = 0xAA;
constexpr unsigned long long kSourceBase = 0x2eea480ull;   // original source cell
constexpr unsigned long long kDestBase = 0x2f21520ull;     // original destination cell

sotc::Leaf575d40Cell pattern(unsigned seed) {
    sotc::Leaf575d40Cell c{};
    for (std::size_t i = 0; i != c.size(); ++i) c[i] = static_cast<std::uint8_t>(seed * 31u + i * 7u + 1u);
    return c;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    for (unsigned seed = 0; seed != 8; ++seed) {
        const Leaf575d40Cell src = pattern(seed);
        const Leaf575d40Cell src_copy = src;
        Leaf575d40Cell dst{};
        dst.fill(kPoison);
        init_leaf_575d40(src, dst);
        for (std::size_t i = 0; i != 20; ++i) CHECK(dst[i] == src[i]);   // byte-wise oracle
        CHECK(src == src_copy);                                          // source untouched
    }
    // Aliasing: source and destination are the same object -> unchanged.
    Leaf575d40Cell same = pattern(3);
    const Leaf575d40Cell same_before = same;
    init_leaf_575d40(same, same);
    CHECK(same == same_before);
    // Zero source (the value of fresh BSS) clears the destination.
    Leaf575d40Cell zero{};
    Leaf575d40Cell dz = pattern(5);
    init_leaf_575d40(zero, dz);
    CHECK(dz == Leaf575d40Cell{});

    if (argc > 1) {
        const Leaf575d40Cell src = pattern(1);
        Leaf575d40Cell z{}, p{};
        p.fill(kPoison);
        init_leaf_575d40(src, z);
        init_leaf_575d40(src, p);
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"", 0x575d40u, 29u, kDestBase);
        for (auto byte : z) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : p) std::printf("%02x", byte);
        std::printf("\",\"sources\":[{\"addr\":%llu,\"bytes\":\"", kSourceBase);
        for (auto byte : src) std::printf("%02x", byte);
        std::printf("\"}],\"addresses\":{}}\n");
    } else {
        std::puts("leaf_575d40: all checks passed");
    }
    return 0;
}
