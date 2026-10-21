// The table clear is compared with an independent coverage oracle (byte i is written iff it lies in
// the first 28 bytes of one of the 256 records, or in the trailing dword) over a poisoned buffer,
// then over a buffer of non-zero bytes to prove the covered bytes become zero. --dump feeds
// tools/lift/check_f9b75a0.py (binary-vs-native tie).
#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "core/leaf_9b75a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
constexpr std::uint8_t kPoison = 0xAA;
constexpr std::size_t kSize = sotc::kLeaf9b75a0Cells;
// Offsets are relative to table+8.
bool covered(std::size_t i) {
    if (i >= 0x27f8 && i < 0x27fc) return true;
    return i / 0x28 < 256 && i % 0x28 < 0x1c;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    CHECK(kSize == 0x27fc);
    std::size_t n = 0;
    for (std::size_t i = 0; i != kSize; ++i) n += covered(i);
    CHECK(n == 256 * 28 + 4);

    Leaf9b75a0 z{};
    init_leaf_9b75a0(z);
    for (auto byte : z.cells) CHECK(byte == 0);

    auto p = std::make_unique<Leaf9b75a0>();
    p->cells.fill(kPoison);
    init_leaf_9b75a0(*p);
    for (std::size_t i = 0; i != kSize; ++i) CHECK(p->cells[i] == (covered(i) ? 0 : kPoison));
    init_leaf_9b75a0(*p);   // idempotent
    for (std::size_t i = 0; i != kSize; ++i) CHECK(p->cells[i] == (covered(i) ? 0 : kPoison));
    // Spot checks at record boundaries: window end, gap, next window start, and the table tail.
    CHECK(p->cells[0x1b] == 0 && p->cells[0x1c] == kPoison && p->cells[0x27] == kPoison && p->cells[0x28] == 0);
    CHECK(p->cells[255 * 0x28 + 0x1b] == 0 && p->cells[255 * 0x28 + 0x1c] == kPoison);
    CHECK(p->cells[0x27f4] == kPoison && p->cells[0x27f7] == kPoison && p->cells[0x27f8] == 0 && p->cells[0x27fb] == 0);

    // Non-zero prefill: covered bytes become zero, the rest keep their value.
    auto q = std::make_unique<Leaf9b75a0>();
    for (std::size_t i = 0; i != kSize; ++i) q->cells[i] = static_cast<std::uint8_t>(1 + (i % 251));
    init_leaf_9b75a0(*q);
    for (std::size_t i = 0; i != kSize; ++i)
        CHECK(q->cells[i] == (covered(i) ? 0 : static_cast<std::uint8_t>(1 + (i % 251))));

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"base\":%llu,\"bytes\":\"", 0x9b75a0u, 50u,
                    static_cast<unsigned long long>(0x32ef798ull));
        for (auto byte : z.cells) std::printf("%02x", byte);
        std::printf("\",\"bytes_poisoned\":\"");
        for (auto byte : p->cells) std::printf("%02x", byte);
        std::printf("\",\"sources\":[],\"addresses\":{}}\n");
    } else {
        std::puts("leaf_9b75a0: all checks passed");
    }
    return 0;
}
