// Independent oracle: the decoded loop makes table[i] == i (int32 identity, 512 entries) and the
// mask cell is {~0,~0,~0,0}. The reconstruction below is written from the loop's semantics, not from
// the implementation's code. --dump feeds tools/lift/check_fd5a280.py (binary-vs-native tie).
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/avx_d5a280.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
std::uint32_t read_u32(const std::uint8_t* p) {
    std::uint32_t v = 0;
    for (int i = 0; i != 4; ++i) v |= static_cast<std::uint32_t>(p[i]) << (8 * i);
    return v;
}
}  // namespace

int main(int argc, char**) {
    using namespace sotc;
    AvxD5a280 g{};
    init_avx_d5a280(g);

    CHECK(g.prefix[0] == 0xff && g.prefix[11] == 0xff);
    CHECK(g.prefix[12] == 0 && g.prefix[13] == 0 && g.prefix[14] == 0 && g.prefix[15] == 0);

    // Identity-table oracle.
    for (std::uint32_t i = 0; i != 512; ++i) CHECK(read_u32(g.table.data() + 4 * i) == i);

    // The four blocks of each iteration differ by 8/16/24 in every lane.
    for (int i = 0; i != 16; ++i) {
        for (int lane = 0; lane != 8; ++lane) {
            const std::uint32_t v = read_u32(g.table.data() + 0x80 * i + 4 * lane);
            CHECK(read_u32(g.table.data() + 0x80 * i + 0x20 + 4 * lane) == v + 8);
            CHECK(read_u32(g.table.data() + 0x80 * i + 0x40 + 4 * lane) == v + 16);
            CHECK(read_u32(g.table.data() + 0x80 * i + 0x60 + 4 * lane) == v + 24);
        }
    }

    AvxD5a280 h = g;
    h.table.fill(0xAA);
    h.prefix.fill(0xAA);
    init_avx_d5a280(h);
    CHECK(std::memcmp(h.table.data(), g.table.data(), g.table.size()) == 0);
    CHECK(std::memcmp(h.prefix.data(), g.prefix.data(), g.prefix.size()) == 0);

    if (argc > 1) {
        std::printf("{\"rva\":%u,\"body_length\":%u,\"prefix_base\":%llu,\"table_base\":%llu,\"bytes\":\"",
                    0xd5a280u, 180u, static_cast<unsigned long long>(0x3352120ull),
                    static_cast<unsigned long long>(0x3352130ull));
        for (auto byte : g.prefix) std::printf("%02x", byte);
        for (auto byte : g.table) std::printf("%02x", byte);
        std::printf("\"}\n");
    } else {
        std::puts("avx_d5a280: all checks passed");
    }
    return 0;
}
