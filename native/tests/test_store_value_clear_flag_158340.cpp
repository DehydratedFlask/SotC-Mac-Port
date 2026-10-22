#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "core/store_value_clear_flag_158340.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

constexpr std::size_t kBuf = 16;

void dump() {
    const std::uint32_t values[] = {0, 1, 0xffffffffu, 0x80000000u, 0x7fffffffu, 0x12345678u, 0xdeadbeefu};
    std::printf("{\"rva\":%u,\"body_length\":7,\"buffer\":%zu,\"cases\":[", 0x158340u, kBuf);
    bool first = true;
    for (std::uint32_t v : values) {
        // Two preimages: all-0xAA and all-0x55, so every byte outside 0..4 must keep its value and
        // byte 4 must become 0 regardless of its previous contents.
        for (unsigned char fill : {0xAAu, 0x55u}) {
            unsigned char b[kBuf];
            std::memset(b, fill, kBuf);
            sotc::store_value_clear_flag_158340(b, v);
            std::printf("%s{\"value\":%u,\"fill\":%u,\"bytes\":\"", first ? "" : ",", v, fill);
            for (unsigned char c : b) std::printf("%02x", c);
            std::printf("\"}");
            first = false;
        }
    }
    std::printf("]}\n");
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--dump") { dump(); return 0; }

    unsigned char b[kBuf];
    std::memset(b, 0xCC, kBuf);
    sotc::store_value_clear_flag_158340(b, 0x01020304u);
    CHECK(b[0] == 0x04 && b[1] == 0x03 && b[2] == 0x02 && b[3] == 0x01);   // little endian dword
    CHECK(b[4] == 0);
    for (std::size_t i = 5; i < kBuf; ++i) CHECK(b[i] == 0xCC);

    // Flag byte is cleared even when it was non-zero; calling again overwrites the value.
    b[4] = 0xFF;
    sotc::store_value_clear_flag_158340(b, 0xffffffffu);
    CHECK(b[0] == 0xff && b[1] == 0xff && b[2] == 0xff && b[3] == 0xff && b[4] == 0);
    sotc::store_value_clear_flag_158340(b, 0);
    CHECK(b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0 && b[4] == 0 && b[5] == 0xCC);

    // Unaligned object (offset 1 of a buffer) works: the store is byte-exact, no alignment demanded.
    std::memset(b, 0xCC, kBuf);
    sotc::store_value_clear_flag_158340(b + 1, 0xA1B2C3D4u);
    CHECK(b[0] == 0xCC && b[1] == 0xD4 && b[4] == 0xA1 && b[5] == 0 && b[6] == 0xCC);

    std::puts("store_value_clear_flag_158340: all checks passed");
    return 0;
}
